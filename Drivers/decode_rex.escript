#!/opt/erlang/erlang-current/bin/escript

main([RexPath]) ->
    {ok, B} = file:read_file(RexPath),
    % Look for packages.
    [_Head | Candidates] = binary:split(B, <<"package1">>, [global]),
    lists:foreach(fun(PkgCandidate) ->
        extract_package(list_to_binary([<<"package1">>, PkgCandidate]))
    end, Candidates).

extract_package(Package) ->
    <<"package1", Reserved1:32, Flags:32, Version:32, CopyrightOffset:16, CopyrightLen:16, NameOffset:16, NameLen:16, TotalSize:32, CreationDate:32, Reserved2:32, Reserved3:32, DirectorySize:32, NumParts:32, Tail1/binary>> = Package,
    VariableLenArea = skip_package_parts(Tail1, NumParts),
    NameUCS2 = binary_part(VariableLenArea, NameOffset, NameLen),
    NameStr = unicode:characters_to_list(NameUCS2, utf16) -- [0],
    Filename0 = re:replace(NameStr, ":", "_", [global, {return, list}]),
    PackageBin = binary_part(Package, 0, TotalSize),
    file:write_file(Filename0 ++ ".pkg", PackageBin),
    ok.

skip_package_parts(Bin, 0) -> Bin;
skip_package_parts(<<_Offset:32, Size:32, Size:32, _Type:32, _Reserved1:32, _Flags:32, _InfoOffset:16, _InfoLen:16, _Reserved2:32, Tail/binary>>, NumParts) ->
    skip_package_parts(Tail, NumParts - 1).
