    .arch armv3

    .macro JT symbol, address
    .section ".text.\symbol\()", "ax"
    .global \symbol
\symbol\():
    .balign 4
    ldr pc, [pc, #-4]
    .word 0x\address
    .endm

    .macro D0DtorGlue symbol, address
    .section ".text.\symbol\()", "ax"
    .global \symbol
\symbol\():
    .balign 4
    mov r1, #1
    ldr pc, [pc, #-4]
    .word 0x\address
    .endm

    .macro D1DtorGlue symbol, address
    .section ".text.\symbol\()", "ax"
    .global \symbol
\symbol\():
    .balign 4
    mov r1, #0
    ldr pc, [pc, #-4]
    .word 0x\address
    .endm

    .macro D2DtorGlue symbol, address
    D1DtorGlue \symbol\(), \address\()
    .endm

    .macro DtorGlue symbol, address
    D0DtorGlue \symbol\()D0Ev, \address\()
    D1DtorGlue \symbol\()D1Ev, \address\()
    D2DtorGlue \symbol\()D2Ev, \address\()
    .endm

    .macro StaticNew class
    .section ".text.New__\class\()Fv", "ax"
    .global New__\class\()Fv
New__\class\()Fv:
    .balign 4
    mov r0, #0
    b __ct__\class\()Fv
    .endm

    JT _ZN11TSerialChip3NewEPc, 00384B0C
    JT _ZN19PSerialChipRegistry8RegisterEP11TSerialChipm, 00384E88
    JT _ZN19PSerialChipRegistry10UnRegisterEP11TSerialChip, 00384e94
    JT _ZN19PSerialChipRegistry10GetChipPtrEm, 00384EAC
    JT _ZN19PSerialChipRegistry14FindByLocationEm, 00384EDC
    JT _ZN11TSerialChip7PutByteEh, 00384B6C
    JT _ZN11TSerialChip13ProcessOptionEP7TOption, 00384C8C
    JT _ZN11TSerialChip18InstallChipHandlerEPvP14SCCChannelInts, 00384B54
    JT _ZN11TSerialChip9RxBufFullEv, 00384B9C
    JT _ZN11TSerialChip7GetByteEv, 00384B84
    JT _ZN11TSerialChip18SetInterruptEnableEh, 00384C14
    JT _ZN11TSerialChip6DeleteEv, 00384B38
    JT _ZN7TOptionC2Em, 0014AA38
    JT _ZN11TUTaskWorldC2Ev, 01BCC670
    JT _ZN11TUTaskWorld9StartTaskEhhmmmm, 01BDCE4C
    JT _ZN14TUAsyncMessageC1Emm, 01BCD6A8
    JT _ZN14TUAsyncMessageC2Ev, 01BCD6AC
    JT _ZN14TAEventHandlerC2Ev, 01BE4208
    JT _Z15NSSendIfDefinedRK6RefVarS1_, 01C03198
    JT _Z15NSSendIfDefinedRK6RefVarS1_S1_, 01C0319C
    JT _Z15NSSendIfDefinedRK6RefVarS1_S1_S1_, 01C031A0
    JT _ZN8TUObject10CopyObjectEm, 01BD0848
    JT _ZN7TAEventC1Ev, 01BE4238
    JT SMemMsgMsgDoneSWI, 003AE4A0
