#!/opt/erlang/erlang-current/bin/escript

-mode(compile).

main([RexPath, "-o", OutDir]) ->
    {ok, B} = file:read_file(RexPath),
    <<"RExBlock", Checksum:32, HeaderVersion:32, Manufacturer:32, Version:32, Size:32, ID:32, StartAddr:32, Entries:32, Tail/binary>> = B,
    Basname = filename:basename(RexPath),
    {ok, SourceFile} = file:open(filename:join(OutDir, Basname ++ ".src"), [write, exclusive]),
    HeaderVersion = 1,
    io:format(SourceFile, "id                ~B\r", [ID]),
    io:format(SourceFile, "version           ~B\r", [Version]),
    io:format(SourceFile, "start             0x~8.16.0B\r", [StartAddr]),
    io:format(SourceFile, "// checksum       0x~8.16.0B\r", [Checksum]),
    % Manufacturer is "Eins" for Einstein.rex, 01000000 for system ROm.
    case Manufacturer of
        1164537459 ->
            io:format(SourceFile, "manufacturer      'Eins'\r", []);
        _ ->
            io:format(SourceFile, "manufacturer      0x~8.16.0B\r", [Manufacturer])
    end,
    Size = byte_size(B),
    decode_rex_entries(SourceFile, OutDir, StartAddr, B, Tail, 0, Entries).

decode_rex_entries(_SourceFile, _OutDir, _StartAddr, _Binary, _Tail, Ix, Ix) -> ok;
decode_rex_entries(SourceFile, OutDir, StartAddr, Binary, <<EntryType:4/binary, Offset:32, Size:32, Tail/binary>>, EntryIx, NumEntries) ->
    Entry = binary_part(Binary, Offset, Size),
    decode_rex_entry(SourceFile, OutDir, StartAddr + Offset, EntryType, Entry, EntryIx),
    decode_rex_entries(SourceFile, OutDir, StartAddr, Binary, Tail, EntryIx + 1, NumEntries).

decode_rex_entry(SourceFile, _OutDir, _Offset, <<"fdrv">>, <<Value:64>>, _EntryIx) ->
    io:format(SourceFile, "block             'fdrv' <~16.16.0B>\r", [Value]);
decode_rex_entry(SourceFile, OutDir, _Offset, <<"FDRV">>, FDRV, EntryIx) ->
    ok = file:write_file(filename:join(OutDir, "RexFDRV" ++ integer_to_list(EntryIx) ++ ".bin"), FDRV),
    io:format(SourceFile, "block             'FDRV' \":RexFDRV~B.bin\"\r", [EntryIx]);
decode_rex_entry(SourceFile, OutDir, Offset, <<"pkgl">>, Entry, _EntryIx) ->
    decode_package_list(SourceFile, OutDir, Offset, Entry);
decode_rex_entry(_SourceFile, _OutDir, _Offset, EntryType, Entry, _EntryIx) ->
    io:format("~w\n", [binary:part(Entry, 0, min(200, byte_size(Entry)))]),
    io:format("~s byte_size = ~B\n", [EntryType, byte_size(Entry)]).

decode_package_list(_SourceFile, _OutDir, _Offset, <<>>) -> ok;
decode_package_list(SourceFile, OutDir, Offset, Entry) ->
    {Name, PackageBin, Tail} = extract_package(Entry),
    Filename0 = re:replace(Name, ":", "_", [global, {return, list}]),
    Filename = Filename0 ++ ".pkg",
    RelocatedPackage = relocate_package(PackageBin, Offset),
    ok = file:write_file(filename:join(OutDir, Filename), RelocatedPackage),
    io:format(SourceFile, "package           \":~s\"\r", [Filename]),
    decode_package_list(SourceFile, OutDir, Offset + byte_size(PackageBin), Tail).
 
extract_package(Package) ->
    <<"package", _SigVariant, _Reserved1:32, _Flags:32, _Version:32, _CopyrightOffset:16, _CopyrightLen:16, NameOffset:16, NameLen:16, TotalSize:32, _CreationDate:32, _Reserved2:32, _Reserved3:32, _DirectorySize:32, NumParts:32, Tail1/binary>> = Package,
    VariableLenArea = skip_package_parts(Tail1, NumParts),
    NameUCS2 = binary_part(VariableLenArea, NameOffset, NameLen),
    NameStr = unicode:characters_to_list(NameUCS2, utf16) -- [0],
    {PackageBin, Tail} = split_binary(Package, TotalSize),
    {NameStr, PackageBin, Tail}.

skip_package_parts(Bin, 0) -> Bin;
skip_package_parts(<<_Offset:32, Size:32, Size:32, _Type:32, _Reserved1:32, _Flags:32, _InfoOffset:16, _InfoLen:16, _Reserved2:32, Tail/binary>>, NumParts) ->
    skip_package_parts(Tail, NumParts - 1).

relocate_package(PackageBin, Offset) ->
    <<"package", _SigVariant, _Reserved1:32, Flags:32, _Version:32, _CopyrightOffset:16, _CopyrightLen:16, _NameOffset:16, _NameLen:16, _TotalSize:32, _CreationDate:32, _Reserved2:32, _Reserved3:32, DirectorySize:32, NumParts:32, Tail/binary>> = PackageBin,
    HasNOS2Relocation = Flags band 16#04000000 =/= 0,
    false = HasNOS2Relocation,   % we don't handle NOS 2.0 relocation yet.
    {Directory, PartsData} = split_binary(PackageBin, DirectorySize),
    RelocatedParts = relocate_parts(Tail, DirectorySize, PartsData, NumParts, Offset),
    list_to_binary([Directory, RelocatedParts]).

relocate_parts(_PartsDir, _DirectorySize, PartsData, 0, _Offset) -> PartsData;
relocate_parts(PartsDir, DirectorySize, PartsData, NumEntries, RelocateOffset) ->
    <<Offset:32, Size:32, Size:32, _Type:32, _Reserved1:32, Flags:32, _InfoOffset:16, _InfoLen:16, _Reserved2:32, Tail/binary>> = PartsDir,
    {BeforePart, PartT} = split_binary(PartsData, Offset),
    {Part, AfterPart} = split_binary(PartT, Size),
    Relocated = if
        Flags band 16#1 =/= 0 ->
            relocate_nos_part(Part, RelocateOffset);
        true -> Part
    end,
    RelocatedPartsData = list_to_binary([BeforePart, Relocated, AfterPart]),
    relocate_parts(Tail, DirectorySize, RelocatedPartsData, NumEntries - 1, RelocateOffset).

relocate_nos_part(Part, RelocationOffset) ->
    <<_ObjectSizeInBytes:24, 2#010000:6, _Type:2, 0:31, AlignmentFlag:1, _/binary>> = Part,
    Alignment = if
        AlignmentFlag =:= 1 -> 4;
        true -> 8
    end,
    relocate_nos_object(Part, RelocationOffset, Alignment, []).

relocate_nos_object(<<>>, _RelocationOffset, _Alignment, Acc) ->
    list_to_binary(lists:reverse(Acc));
relocate_nos_object(PartData, RelocationOffset, Alignment, Acc) ->
    <<ObjectSizeInBytes:24, 2#010000:6, Type:2, 0:31, _AlignmentFlag:1, _/binary>> = PartData,
    {Object, AfterObject} = split_binary(PartData, ObjectSizeInBytes),
    {ObjectHeader, ObjectData} = split_binary(Object, 8),
    {Padding, Tail} = case ObjectSizeInBytes rem Alignment of
        0 -> {<<>>, AfterObject};
        NumBytes -> split_binary(AfterObject, Alignment - NumBytes)
    end,
    RelocatedObjectData = relocate_nos_object_data(Type, ObjectData, RelocationOffset),
    RewrittenObject = list_to_binary([ObjectHeader, RelocatedObjectData, Padding]),
    relocate_nos_object(Tail, RelocationOffset, Alignment, [RewrittenObject | Acc]).

relocate_nos_object_data(0, <<ClassRefPtr:30, 2#01:2, BinaryData/binary>>, Offset) ->
    Ptr32 = ClassRefPtr * 4,
    RewrittenPointer = Ptr32 - Offset,
    <<(RewrittenPointer + 1):32, BinaryData/binary>>;
relocate_nos_object_data(0, <<_OtherRef:32, _BinaryData/binary>> = ObjectBin, _Offset) ->
    ObjectBin;
relocate_nos_object_data(Type, Slots, Offset) when Type =:= 2#01 orelse Type =:= 2#11 ->
    relocate_nos_object_slots(Slots, Offset, []).

relocate_nos_object_slots(<<Ptr:30, 2#01:2, Tail/binary>>, Offset, AccRewritten) ->
    Ptr32 = Ptr * 4,
    RewrittenPointer = Ptr32 - Offset,
    relocate_nos_object_slots(Tail, Offset, [<<(RewrittenPointer+1):32>> | AccRewritten]);
relocate_nos_object_slots(<<Ref:32, Tail/binary>>, Offset, AccRewritten) ->
    relocate_nos_object_slots(Tail, Offset, [<<Ref:32>> | AccRewritten]);
relocate_nos_object_slots(<<>>, _Offset, AccRewritten) ->
    list_to_binary(lists:reverse(AccRewritten)).
