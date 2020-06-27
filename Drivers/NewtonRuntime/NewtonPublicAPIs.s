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

/*
C++: TAEventHandler::AECompletionProc(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: AECompletionProc__14TAEventHandlerFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN14TAEventHandler16AECompletionProcEP10TUMsgTokenPmP7TAEvent, 018019a8

/*
C++: TUSoundChannel::AECompletionProc(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: AECompletionProc__14TUSoundChannelFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN14TUSoundChannel16AECompletionProcEP10TUMsgTokenPmP7TAEvent, 01802c20

/*
C++: TEndpointClient::AECompletionProc(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: AECompletionProc__15TEndpointClientFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN15TEndpointClient16AECompletionProcEP10TUMsgTokenPmP7TAEvent, 01800238

/*
C++: TEndpointEventHandler::AECompletionProc(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: AECompletionProc__21TEndpointEventHandlerFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN21TEndpointEventHandler16AECompletionProcEP10TUMsgTokenPmP7TAEvent, 0180023c

/*
C++: TAppWorld::AEDeferReply()
MPW-mangled: AEDeferReply__9TAppWorldFv
*/
    JT _ZN9TAppWorld12AEDeferReplyEv, 018019ac

/*
C++: TAppWorld::AEDispatch(unsigned long, TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: AEDispatch__9TAppWorldFUlP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN9TAppWorld10AEDispatchEmP10TUMsgTokenPmP7TAEvent, 018019b0

/*
C++: TAppWorld::AEFindHandler(unsigned long, unsigned long)
MPW-mangled: AEFindHandler__9TAppWorldFUlT1
*/
    JT AEFindHandler__9TAppWorldFUlT1, 018019b4

/*
C++: TAppWorld::AEGetAEvent()
MPW-mangled: AEGetAEvent__9TAppWorldFv
*/
    JT _ZN9TAppWorld11AEGetAEventEv, 018019b8

/*
C++: TAppWorld::AEGetCollectedEvent(unsigned long, TUMsgToken*, unsigned long*, TAEvent**, unsigned long*)
MPW-mangled: AEGetCollectedEvent__9TAppWorldFUlP10TUMsgTokenPUlPP7TAEventT3
*/
    JT AEGetCollectedEvent__9TAppWorldFUlP10TUMsgTokenPUlPP7TAEventT3, 018019bc

/*
C++: TAppWorld::AEGetMsgSize()
MPW-mangled: AEGetMsgSize__9TAppWorldFv
*/
    JT _ZN9TAppWorld12AEGetMsgSizeEv, 018019c0

/*
C++: TAppWorld::AEGetMsgToken()
MPW-mangled: AEGetMsgToken__9TAppWorldFv
*/
    JT _ZN9TAppWorld13AEGetMsgTokenEv, 018019c4

/*
C++: TAppWorld::AEGetMsgType()
MPW-mangled: AEGetMsgType__9TAppWorldFv
*/
    JT _ZN9TAppWorld12AEGetMsgTypeEv, 018019c8

/*
C++: TAEventHandler::AEHandlerProc(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: AEHandlerProc__14TAEventHandlerFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN14TAEventHandler13AEHandlerProcEP10TUMsgTokenPmP7TAEvent, 018019cc

/*
C++: TEndpointClient::AEHandlerProc(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: AEHandlerProc__15TEndpointClientFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN15TEndpointClient13AEHandlerProcEP10TUMsgTokenPmP7TAEvent, 01800240

/*
C++: TUserAbortHandler::AEHandlerProc(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: AEHandlerProc__17TUserAbortHandlerFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN17TUserAbortHandler13AEHandlerProcEP10TUMsgTokenPmP7TAEvent, 01800244

/*
C++: TEndpointEventHandler::AEHandlerProc(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: AEHandlerProc__21TEndpointEventHandlerFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN21TEndpointEventHandler13AEHandlerProcEP10TUMsgTokenPmP7TAEvent, 01800248

/*
C++: TAppWorld::AEInstallHandler(TAEventHandler*)
MPW-mangled: AEInstallHandler__9TAppWorldFP14TAEventHandler
*/
    JT _ZN9TAppWorld16AEInstallHandlerEP14TAEventHandler, 018019d0

/*
C++: TAppWorld::AEInstallIdleHandler(TAEventHandler*)
MPW-mangled: AEInstallIdleHandler__9TAppWorldFP14TAEventHandler
*/
    JT _ZN9TAppWorld20AEInstallIdleHandlerEP14TAEventHandler, 018019d4

/*
C++: TAppWorld::AERemoveHandler(TAEventHandler*)
MPW-mangled: AERemoveHandler__9TAppWorldFP14TAEventHandler
*/
    JT _ZN9TAppWorld15AERemoveHandlerEP14TAEventHandler, 018019d8

/*
C++: TAppWorld::AERemoveIdleHandler(TAEventHandler*)
MPW-mangled: AERemoveIdleHandler__9TAppWorldFP14TAEventHandler
*/
    JT _ZN9TAppWorld19AERemoveIdleHandlerEP14TAEventHandler, 018019dc

/*
C++: TAppWorld::AEReplyImmed()
MPW-mangled: AEReplyImmed__9TAppWorldFv
*/
    JT _ZN9TAppWorld12AEReplyImmedEv, 018019e0

/*
C++: TAppWorld::AESetReply(TUMsgToken*)
MPW-mangled: AESetReply__9TAppWorldFP10TUMsgToken
*/
    JT _ZN9TAppWorld10AESetReplyEP10TUMsgToken, 018019e4

/*
C++: TAppWorld::AESetReply(TUMsgToken*, unsigned long, TAEvent*)
MPW-mangled: AESetReply__9TAppWorldFP10TUMsgTokenUlP7TAEvent
*/
    JT _ZN9TAppWorld10AESetReplyEP10TUMsgTokenmP7TAEvent, 018019e8

/*
C++: TAppWorld::AESetReply(unsigned long)
MPW-mangled: AESetReply__9TAppWorldFUl
*/
    JT _ZN9TAppWorld10AESetReplyEm, 018019ec

/*
C++: TAppWorld::AESetReply(unsigned long, TAEvent*)
MPW-mangled: AESetReply__9TAppWorldFUlP7TAEvent
*/
    JT _ZN9TAppWorld10AESetReplyEmP7TAEvent, 018019f0

/*
C++: TAppWorld::AETerminateLoop()
MPW-mangled: AETerminateLoop__9TAppWorldFv
*/
    JT _ZN9TAppWorld15AETerminateLoopEv, 018019f4

/*
C++: TAEventHandler::AETestEvent(TAEvent*)
MPW-mangled: AETestEvent__14TAEventHandlerFP7TAEvent
*/
    JT _ZN14TAEventHandler11AETestEventEP7TAEvent, 018019f8

/*
C++: TEndpointClient::AETestEvent(TAEvent*)
MPW-mangled: AETestEvent__15TEndpointClientFP7TAEvent
*/
    JT _ZN15TEndpointClient11AETestEventEP7TAEvent, 0180024c

/*
C++: TUserAbortHandler::AETestEvent(TAEvent*)
MPW-mangled: AETestEvent__17TUserAbortHandlerFP7TAEvent
*/
    JT _ZN17TUserAbortHandler11AETestEventEP7TAEvent, 01800250

/*
C++: TEndpointEventHandler::AETestEvent(TAEvent*)
MPW-mangled: AETestEvent__21TEndpointEventHandlerFP7TAEvent
*/
    JT _ZN21TEndpointEventHandler11AETestEventEP7TAEvent, 01800254

/*
C++: TAppWorld::AEventLoop(TAppWorldState*)
MPW-mangled: AEventLoop__9TAppWorldFP14TAppWorldState
*/
    JT _ZN9TAppWorld10AEventLoopEP14TAppWorldState, 018019fc

/*
C++: TAppWorld::AEventLoop(TAppWorldState*, TUMsgToken*)
MPW-mangled: AEventLoop__9TAppWorldFP14TAppWorldStateP10TUMsgToken
*/
    JT _ZN9TAppWorld10AEventLoopEP14TAppWorldStateP10TUMsgToken, 01801a00

/*
C++: TAppWorld::AEventLoop()
MPW-mangled: AEventLoop__9TAppWorldFv
*/
    JT _ZN9TAppWorld10AEventLoopEv, 01801a04

/*
C++: ASCIIString(const RefVar&)
MPW-mangled: ASCIIString__FRC6RefVar
*/
    JT _Z11ASCIIStringRK6RefVar, 01800824

/*
C++: TATA::ATASpecific(unsigned long, void*, unsigned long)
MPW-mangled: ATASpecific__4TATAFUlPvT1
*/
    JT ATASpecific__4TATAFUlPvT1, 01802a60

/*
C++: TUSoundChannel::AbortBusy()
MPW-mangled: AbortBusy__14TUSoundChannelFv
*/
    JT _ZN14TUSoundChannel9AbortBusyEv, 01802c24

/*
C++: TUAsyncMessage::Abort(TUMsgToken*, unsigned long*)
MPW-mangled: Abort__14TUAsyncMessageFP10TUMsgTokenPUl
*/
    JT _ZN14TUAsyncMessage5AbortEP10TUMsgTokenPm, 01800e14

/*
C++: TUAsyncMessage::Abort()
MPW-mangled: Abort__14TUAsyncMessageFv
*/
    JT _ZN14TUAsyncMessage5AbortEv, 01800e18

/*
C++: TEzEndpointPipe::Abort()
MPW-mangled: Abort__15TEzEndpointPipeFv
*/
    JT _ZN15TEzEndpointPipe5AbortEv, 01800214

/*
C++: TEndpointEventHandler::Abort()
MPW-mangled: Abort__21TEndpointEventHandlerFv
*/
    JT _ZN21TEndpointEventHandler5AbortEv, 01800218

/*
C++: TEndpoint::Abort()
MPW-mangled: Abort__9TEndpointFv
*/
    JT _ZN9TEndpoint5AbortEv, 0180021c

/*
C++: TEndpointClient::AcceptComplete(TEndpointEvent*)
MPW-mangled: AcceptComplete__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient14AcceptCompleteEP14TEndpointEvent, 01800224

/*
C++: TCommTool::AcceptComplete(long)
MPW-mangled: AcceptComplete__9TCommToolFl
*/
    JT _ZN9TCommTool14AcceptCompleteEl, 0180051c

/*
C++: AcceptObject(unsigned long)
MPW-mangled: AcceptObject__FUl
*/
    JT _Z12AcceptObjectm, 01800e1c

/*
C++: TCommTool::AcceptStart()
MPW-mangled: AcceptStart__9TCommToolFv
*/
    JT _ZN9TCommTool11AcceptStartEv, 01800520

/*
C++: TCommTool::Accept()
MPW-mangled: Accept__9TCommToolFv
*/
    JT _ZN9TCommTool6AcceptEv, 01800518

/*
C++: TEndpoint::Accept(TEndpoint*, TOptionArray*, TOptionArray*, CBufferSegment*, long, unsigned long)
MPW-mangled: Accept__9TEndpointFP9TEndpointP12TOptionArrayT2P14CBufferSegmentlUl
*/
    JT Accept__9TEndpointFP9TEndpointP12TOptionArrayT2P14CBufferSegmentlUl, 01800220

/*
C++: TNSDebugAPI::AccurateStack()
MPW-mangled: AccurateStack__11TNSDebugAPIFv
*/
    JT _ZN11TNSDebugAPI13AccurateStackEv, 018027ec

/*
C++: TURdWrSemaphore::AcquireRd(SemFlags)
MPW-mangled: AcquireRd__15TURdWrSemaphoreF8SemFlags
*/
    JT _ZN15TURdWrSemaphore9AcquireRdE8SemFlags, 01800e24

/*
C++: TURdWrSemaphore::AcquireWr(SemFlags)
MPW-mangled: AcquireWr__15TURdWrSemaphoreF8SemFlags
*/
    JT _ZN15TURdWrSemaphore9AcquireWrE8SemFlags, 01800e28

/*
C++: TULockingSemaphore::Acquire(SemFlags)
MPW-mangled: Acquire__18TULockingSemaphoreF8SemFlags
*/
    JT _ZN18TULockingSemaphore7AcquireE8SemFlags, 01800e20

/*
C++: AddArraySlot(const RefVar&, const RefVar&)
MPW-mangled: AddArraySlot__FRC6RefVarT1
*/
    JT _Z12AddArraySlotRK6RefVarS1_, 018007f4

/*
C++: TDoubleQContainer::AddBefore(void*, void*)
MPW-mangled: AddBefore__17TDoubleQContainerFPvT1
*/
    JT AddBefore__17TDoubleQContainerFPvT1, 01800e38

/*
C++: TCardPCMCIA::AddCardConfiguration(TCardConfiguration*)
MPW-mangled: AddCardConfiguration__11TCardPCMCIAFP18TCardConfiguration
*/
    JT _ZN11TCardPCMCIA20AddCardConfigurationEP18TCardConfiguration, 01800e3c

/*
C++: TCardPCMCIA::AddCardDevice(TCardDevice*)
MPW-mangled: AddCardDevice__11TCardPCMCIAFP11TCardDevice
*/
    JT _ZN11TCardPCMCIA13AddCardDeviceEP11TCardDevice, 01800e40

/*
C++: TCardPCMCIA::AddCardFunction(TCardFunction*)
MPW-mangled: AddCardFunction__11TCardPCMCIAFP13TCardFunction
*/
    JT _ZN11TCardPCMCIA15AddCardFunctionEP13TCardFunction, 01802dd4

/*
C++: TCardPCMCIA::AddCardOtherCondDeviceList(const unsigned long)
MPW-mangled: AddCardOtherCondDeviceList__11TCardPCMCIAFCUl
*/
    JT _ZN11TCardPCMCIA26AddCardOtherCondDeviceListEKm, 01802dc4

/*
C++: TCardPCMCIA::AddCardOtherCondDevice(const unsigned long, const unsigned long, TCardDevice*)
MPW-mangled: AddCardOtherCondDevice__11TCardPCMCIAFCUlT1P11TCardDevice
*/
    JT AddCardOtherCondDevice__11TCardPCMCIAFCUlT1P11TCardDevice, 01802dc8

/*
C++: TCardPCMCIA::AddCardPackage(TCardPackage*)
MPW-mangled: AddCardPackage__11TCardPCMCIAFP12TCardPackage
*/
    JT _ZN11TCardPCMCIA14AddCardPackageEP12TCardPackage, 01800e44

/*
C++: TView::AddChild(const RefVar&)
MPW-mangled: AddChild__5TViewFRC6RefVar
*/
    JT _ZN5TView8AddChildERK6RefVar, 01800b60

/*
C++: TCommTool::AddCurrentOptions(TOptionArray*)
MPW-mangled: AddCurrentOptions__9TCommToolFP12TOptionArray
*/
    JT _ZN9TCommTool17AddCurrentOptionsEP12TOptionArray, 01800524

/*
C++: TCommTool::AddDefaultOptions(TOptionArray*)
MPW-mangled: AddDefaultOptions__9TCommToolFP12TOptionArray
*/
    JT _ZN9TCommTool17AddDefaultOptionsEP12TOptionArray, 01800528

/*
C++: AddDomainToEnvironment(unsigned long, unsigned long, unsigned long)
MPW-mangled: AddDomainToEnvironment__FUlN21
*/
    JT AddDomainToEnvironment__FUlN21, 01800e50

/*
C++: TUDomainManager::AddDomain(unsigned long&, unsigned long, unsigned long)
MPW-mangled: AddDomain__15TUDomainManagerFRUlUlT2
*/
    JT AddDomain__15TUDomainManagerFRUlUlT2, 01800e48

/*
C++: TUDomainManager::AddDomain(unsigned long)
MPW-mangled: AddDomain__15TUDomainManagerFUl
*/
    JT _ZN15TUDomainManager9AddDomainEm, 01800e4c
    JT AddExceptionHandler, 01801748

/*
C++: TCardFunction::AddFuncExt(const unsigned long, const unsigned char*)
MPW-mangled: AddFuncExt__13TCardFunctionFCUlPCUc
*/
    JT _ZN13TCardFunction10AddFuncExtEKmPKh, 01802db8

/*
C++: TCardPCMCIA::AddFuncSpecificCIS()
MPW-mangled: AddFuncSpecificCIS__11TCardPCMCIAFv
*/
    JT _ZN11TCardPCMCIA18AddFuncSpecificCISEv, 01802dd8

/*
C++: AddGCRoot(long&)
MPW-mangled: AddGCRoot__FRl
*/
    JT _Z9AddGCRootRl, 018007f8
    JT AddSemaphoreToHeap, 0180174c

/*
C++: AddSlot(const RefVar&, const RefVar&)
MPW-mangled: AddSlot__FRC6RefVarT1
*/
    JT AddSlot__FRC6RefVarT1, 018007fc

/*
C++: TWRecognizer::AddSub(TWRecUnit*, TStrokeUnit*)
MPW-mangled: AddSub__12TWRecognizerFP9TWRecUnitP11TStrokeUnit
*/
    JT _ZN12TWRecognizer6AddSubEP9TWRecUnitP11TStrokeUnit, 01802ce8

/*
C++: TEndpointPipe::AddToAppWorld()
MPW-mangled: AddToAppWorld__13TEndpointPipeFv
*/
    JT _ZN13TEndpointPipe13AddToAppWorldEv, 01800228

/*
C++: TEndpointEventHandler::AddToAppWorld()
MPW-mangled: AddToAppWorld__21TEndpointEventHandlerFv
*/
    JT _ZN21TEndpointEventHandler13AddToAppWorldEv, 0180022c

/*
C++: TEndpoint::AddToAppWorld()
MPW-mangled: AddToAppWorld__9TEndpointFv
*/
    JT _ZN9TEndpoint13AddToAppWorldEv, 01800230

/*
C++: TDoubleQContainer::AddToFront(void*)
MPW-mangled: AddToFront__17TDoubleQContainerFPv
*/
    JT _ZN17TDoubleQContainer10AddToFrontEPv, 01800e54

/*
C++: TView::AddToSoup(const RefVar&)
MPW-mangled: AddToSoup__5TViewFRC6RefVar
*/
    JT _ZN5TView9AddToSoupERK6RefVar, 01800b64

/*
C++: TView::AddView(TView*)
MPW-mangled: AddView__5TViewFP5TView
*/
    JT _ZN5TView7AddViewEP5TView, 01800b68

/*
C++: TView::AddViews(unsigned char)
MPW-mangled: AddViews__5TViewFUc
*/
    JT _ZN5TView8AddViewsEh, 01800b6c

/*
C++: TWRecognizer::AddWordInterpretation(TWRecUnit*)
MPW-mangled: AddWordInterpretation__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer21AddWordInterpretationEP9TWRecUnit, 01802cec

/*
C++: TSoupIndex::Add(SKey*, SKey*)
MPW-mangled: Add__10TSoupIndexFP4SKeyT1
*/
    JT Add__10TSoupIndexFP4SKeyT1, 018007f0

/*
C++: TUEnvironment::Add(unsigned long, unsigned char, unsigned char, unsigned char)
MPW-mangled: Add__13TUEnvironmentFUlUcN22
*/
    JT Add__13TUEnvironmentFUlUcN22, 01800e2c

/*
C++: TDoubleQContainer::Add(void*)
MPW-mangled: Add__17TDoubleQContainerFPv
*/
    JT _ZN17TDoubleQContainer3AddEPv, 01800e30

/*
C++: TUCTTable::Add(TTask*, unsigned long)
MPW-mangled: Add__9TUCTTableFP5TTaskUl
*/
    JT _ZN9TUCTTable3AddEP5TTaskm, 01800e34

/*
C++: CArrayIterator::Advance()
MPW-mangled: Advance__14CArrayIteratorFv
*/
    JT _ZN14CArrayIterator7AdvanceEv, 018019a0

/*
C++: TOptionIterator::Advance()
MPW-mangled: Advance__15TOptionIteratorFv
*/
    JT _ZN15TOptionIterator7AdvanceEv, 01800234

/*
C++: TAEHandlerIterator::Advance()
MPW-mangled: Advance__18TAEHandlerIteratorFv
*/
    JT _ZN18TAEHandlerIterator7AdvanceEv, 018019a4

/*
C++: static TRealTimeClock::AlarmStatus(unsigned long, unsigned long*, TTime*, long*)
MPW-mangled: AlarmStatus__14TRealTimeClockSFUlPUlP5TTimePl
*/
    JT _ZN14TRealTimeClock11AlarmStatusEmPmP5TTimePl, 01800e5c

/*
C++: static TURealTimeAlarm::AlarmStatus(unsigned long, unsigned char*, TTime*, long*)
MPW-mangled: AlarmStatus__15TURealTimeAlarmSFUlPUcP5TTimePl
*/
    JT _ZN15TURealTimeAlarm11AlarmStatusEmPhP5TTimePl, 01800e60

/*
C++: static TRealTimeClock::Alarm()
MPW-mangled: Alarm__14TRealTimeClockSFv
*/
    JT _ZN14TRealTimeClock5AlarmEv, 01800e58

/*
C++: TAlertDialog::Alert(unsigned long*)
MPW-mangled: Alert__12TAlertDialogFPUl
*/
    JT _ZN12TAlertDialog5AlertEPm, 01802790

/*
C++: TUPhys::Align(unsigned long&)
MPW-mangled: Align__6TUPhysFRUl
*/
    JT _ZN6TUPhys5AlignERm, 01800e64

/*
C++: TPoint::Aligned(const TPoint&, const TPoint&) const
MPW-mangled: Aligned__6TPointCFRC6TPointT1
*/
    JT Aligned__6TPointCFRC6TPointT1, 01800b70

/*
C++: AllocInstanceByName(const char*, const char*)
MPW-mangled: AllocInstanceByName__FPCcT1
*/
    JT _Z19AllocInstanceByNamePKcS0_, 01800e70

/*
C++: TClassInfo::AllocProc() const
MPW-mangled: AllocProc__10TClassInfoCFv
*/
    JT AllocProc__10TClassInfoCFv, 01800e74

/*
C++: AllocateArray(const RefVar&, long)
MPW-mangled: AllocateArray__FRC6RefVarl
*/
    JT _Z13AllocateArrayRK6RefVarl, 01800804

/*
C++: AllocateBinary(const RefVar&, long)
MPW-mangled: AllocateBinary__FRC6RefVarl
*/
    JT _Z14AllocateBinaryRK6RefVarl, 01800808

/*
C++: AllocateCObjectBinary(void*, void (*)(void*), void (*)(void*), void (*)(void*))
MPW-mangled: AllocateCObjectBinary__FPvPFPv_vN22
*/
    JT _Z21AllocateCObjectBinaryPvPFvS_ES1_S1_, 018027e0

/*
C++: AllocateFrameWithMap(const RefVar&)
MPW-mangled: AllocateFrameWithMap__FRC6RefVar
*/
    JT _Z20AllocateFrameWithMapRK6RefVar, 01800810

/*
C++: AllocateFrame()
MPW-mangled: AllocateFrame__Fv
*/
    JT _Z13AllocateFramev, 0180080c

/*
C++: TObjectHeap::AllocateIndirectBinary(const RefVar&, long)
MPW-mangled: AllocateIndirectBinary__11TObjectHeapFRC6RefVarl
*/
    JT _ZN11TObjectHeap22AllocateIndirectBinaryERK6RefVarl, 01802c8c

/*
C++: AllocateMapWithTags(const RefVar&, const RefVar&)
MPW-mangled: AllocateMapWithTags__FRC6RefVarT1
*/
    JT AllocateMapWithTags__FRC6RefVarT1, 01800814

/*
C++: AllocatePackage(CPipe*, TStore*, unsigned long, char*, void*, long, TCallbackCompressor*)
MPW-mangled: AllocatePackage__FP5CPipeP6TStoreUlPcPvlP19TCallbackCompressor
*/
    JT _Z15AllocatePackageP5CPipeP6TStoremPcPvlP19TCallbackCompressor, 01800e68

/*
C++: static TUDomainManager::AllocatePageTable()
MPW-mangled: AllocatePageTable__15TUDomainManagerSFv
*/
    JT _ZN15TUDomainManager17AllocatePageTableEv, 01800e6c

/*
C++: AllocateRefHandle(long)
MPW-mangled: AllocateRefHandle__Fl
*/
    JT _Z17AllocateRefHandlel, 01800818

/*
C++: TCircleBuf::Allocate(unsigned long)
MPW-mangled: Allocate__10TCircleBufFUl
*/
    JT _ZN10TCircleBuf8AllocateEm, 018029ec

/*
C++: TCircleBuf::Allocate(unsigned long, int, unsigned char, unsigned char)
MPW-mangled: Allocate__10TCircleBufFUliUcT3
*/
    JT Allocate__10TCircleBufFUliUcT3, 018029f0

/*
C++: TCommTool::AllowAbort()
MPW-mangled: AllowAbort__9TCommToolFv
*/
    JT _ZN9TCommTool10AllowAbortEv, 0180052c

/*
C++: AllowSchedule()
MPW-mangled: AllowSchedule__Fv
*/
    JT _Z13AllowSchedulev, 01800e78

/*
C++: TSystemEventHandler::AnySystemEvents(TAEvent*)
MPW-mangled: AnySystemEvents__19TSystemEventHandlerFP7TAEvent
*/
    JT _ZN19TSystemEventHandler15AnySystemEventsEP7TAEvent, 01801a08

/*
C++: AnyVppOn()
MPW-mangled: AnyVppOn__Fv
*/
    JT _Z8AnyVppOnv, 01802a64

/*
C++: ArrayInsert(const RefVar&, const RefVar&, long)
MPW-mangled: ArrayInsert__FRC6RefVarT1l
*/
    JT ArrayInsert__FRC6RefVarT1l, 01802ba8

/*
C++: ArrayMunger(const RefVar&, long, long, const RefVar&, long, long)
MPW-mangled: ArrayMunger__FRC6RefVarlT2T1N22
*/
    JT ArrayMunger__FRC6RefVarlT2T1N22, 0180081c

/*
C++: ArrayPosition(const RefVar&, const RefVar&, long, const RefVar&)
MPW-mangled: ArrayPosition__FRC6RefVarT1lT1
*/
    JT ArrayPosition__FRC6RefVarT1lT1, 01800820

/*
C++: ArrayRemoveCount(const RefVar&, long, long)
MPW-mangled: ArrayRemoveCount__FRC6RefVarlT2
*/
    JT ArrayRemoveCount__FRC6RefVarlT2, 01802b1c

/*
C++: ArrayRemove(const RefVar.global ArrayRemove__FRC6RefVarT1, const RefVar.global ArrayRemove__FRC6RefVarT1)
MPW-mangled: ArrayRemove__FRC6RefVarT1
*/
    JT _Z11ArrayRemoveRK6RefVarS1_, 01802b20

/*
C++: CBufferList::AtEOF() const
MPW-mangled: AtEOF__11CBufferListCFv
*/
    JT _ZNK11CBufferList5AtEOFEv, 01801a14

/*
C++: CRingBuffer::AtEOF() const
MPW-mangled: AtEOF__11CRingBufferCFv
*/
    JT AtEOF__11CRingBufferCFv, 01801a18

/*
C++: CBufferSegment::AtEOF() const
MPW-mangled: AtEOF__14CBufferSegmentCFv
*/
    JT AtEOF__14CBufferSegmentCFv, 01801a1c

/*
C++: CShadowRingBuffer::AtEOF() const
MPW-mangled: AtEOF__17CShadowRingBufferCFv
*/
    JT AtEOF__17CShadowRingBufferCFv, 01801a20

/*
C++: CShadowBufferSegment::AtEOF() const
MPW-mangled: AtEOF__20CShadowBufferSegmentCFv
*/
    JT AtEOF__20CShadowBufferSegmentCFv, 01801a24

/*
C++: CBufferList::At(long)
MPW-mangled: At__11CBufferListFl
*/
    JT _ZN11CBufferList2AtEl, 01801a0c

/*
C++: CList::At(long)
MPW-mangled: At__5CListFl
*/
    JT _ZN5CList2AtEl, 01801a10

/*
C++: TCardSocket::AttributeMemBaseAddr()
MPW-mangled: AttributeMemBaseAddr__11TCardSocketFv
*/
    JT _ZN11TCardSocket20AttributeMemBaseAddrEv, 01800e7c

/*
C++: BSearchLeft(const RefVar&, const RefVar&, TGeneralizedTestFnVar*)
MPW-mangled: BSearchLeft__FRC6RefVarT1P21TGeneralizedTestFnVar
*/
    JT BSearchLeft__FRC6RefVarT1P21TGeneralizedTestFnVar, 01802bac

/*
C++: BSearchRight(const RefVar&, const RefVar&, TGeneralizedTestFnVar*)
MPW-mangled: BSearchRight__FRC6RefVarT1P21TGeneralizedTestFnVar
*/
    JT BSearchRight__FRC6RefVarT1P21TGeneralizedTestFnVar, 01802bb0

/*
C++: BackupPackage(CPipe*, unsigned long)
MPW-mangled: BackupPackage__FP5CPipeUl
*/
    JT _Z13BackupPackageP5CPipem, 01800e84

/*
C++: BackupPatch(unsigned long)
MPW-mangled: BackupPatch__FUl
*/
    JT _Z11BackupPatchm, 01800e88

/*
C++: BackupPatches(CPipe*)
MPW-mangled: BackupPatches__FP5CPipe
*/
    JT _Z13BackupPatchesP5CPipe, 01800e8c

/*
C++: TPartHandler::Backup(const PartId&, long, CPipe*)
MPW-mangled: Backup__12TPartHandlerFRC6PartIdlP5CPipe
*/
    JT _ZN12TPartHandler6BackupERK6PartIdlP5CPipe, 01800e80

/*
C++: TUPhys::Base(unsigned long&)
MPW-mangled: Base__6TUPhysFRUl
*/
    JT _ZN6TUPhys4BaseERm, 01802538

/*
C++: TProtocol::Become(const TProtocol*)
MPW-mangled: Become__9TProtocolFPC9TProtocol
*/
    JT _ZN9TProtocol6BecomeEPK9TProtocol, 01800e94

/*
C++: TProtocol::Become(unsigned long)
MPW-mangled: Become__9TProtocolFUl
*/
    JT _ZN9TProtocol6BecomeEm, 01800e98

/*
C++: BinEqual(const RefVar&, const RefVar&)
MPW-mangled: BinEqual__FRC6RefVarT1
*/
    JT BinEqual__FRC6RefVarT1, 01802b98

/*
C++: BinaryData(long)
MPW-mangled: BinaryData__Fl
*/
    JT _Z10BinaryDatal, 01800828

/*
C++: BinaryMunger(const RefVar&, long, long, const RefVar&, long, long)
MPW-mangled: BinaryMunger__FRC6RefVarlT2T1N22
*/
    JT BinaryMunger__FRC6RefVarlT2T1N22, 0180082c

/*
C++: TCommTool::Bind()
MPW-mangled: Bind__9TCommToolFv
*/
    JT _ZN9TCommTool4BindEv, 01800530

/*
C++: TEndpoint::Bind(TOptionArray*, long*, unsigned long)
MPW-mangled: Bind__9TEndpointFP12TOptionArrayPlUl
*/
    JT _ZN9TEndpoint4BindEP12TOptionArrayPlm, 01800258
    JT BlockMove, 01801620

/*
C++: BlockStatistics(char*, unsigned long*, unsigned char*)
MPW-mangled: BlockStatistics__FPcPUlPUc
*/
    JT _Z15BlockStatisticsPcPmPh, 018027c8

/*
C++: TUAsyncMessage::BlockTillDone(unsigned long*, unsigned long*, unsigned long*, unsigned long*)
MPW-mangled: BlockTillDone__14TUAsyncMessageFPUlN31
*/
    JT BlockTillDone__14TUAsyncMessageFPUlN31, 01800e9c

/*
C++: TLibrarian::BookAvailable(const RefVar&, const RefVar&)
MPW-mangled: BookAvailable__10TLibrarianFRC6RefVarT1
*/
    JT BookAvailable__10TLibrarianFRC6RefVarT1, 01801e58

/*
C++: TLibrarian::BookRemoved(const RefVar&)
MPW-mangled: BookRemoved__10TLibrarianFRC6RefVar
*/
    JT _ZN10TLibrarian11BookRemovedERK6RefVar, 01801e5c

/*
C++: TView::BringToFront()
MPW-mangled: BringToFront__5TViewFv
*/
    JT _ZN5TView12BringToFrontEv, 01800b74

/*
C++: TCircleBuf::BufferCountToNextMarker(unsigned long*)
MPW-mangled: BufferCountToNextMarker__10TCircleBufFPUl
*/
    JT _ZN10TCircleBuf23BufferCountToNextMarkerEPm, 018029f4

/*
C++: TCircleBuf::BufferCount()
MPW-mangled: BufferCount__10TCircleBufFv
*/
    JT _ZN10TCircleBuf11BufferCountEv, 018029f8

/*
C++: TCircleBuf::BufferSpace(unsigned long)
MPW-mangled: BufferSpace__10TCircleBufFUl
*/
    JT _ZN10TCircleBuf11BufferSpaceEm, 018029fc

/*
C++: TCircleBuf::BufferSpace()
MPW-mangled: BufferSpace__10TCircleBufFv
*/
    JT _ZN10TCircleBuf11BufferSpaceEv, 01802a00

/*
C++: TEzEndpointPipe::BytesAvailable()
MPW-mangled: BytesAvailable__15TEzEndpointPipeFv
*/
    JT _ZN15TEzEndpointPipe14BytesAvailableEv, 0180025c

/*
C++: CDouble(const RefVar&)
MPW-mangled: CDouble__FRC6RefVar
*/
    JT _Z7CDoubleRK6RefVar, 01800830

/*
C++: CMGetEndpoint(TOptionArray*, TEndpoint**, unsigned char)
MPW-mangled: CMGetEndpoint__FP12TOptionArrayPP9TEndpointUc
*/
    JT _Z13CMGetEndpointP12TOptionArrayPP9TEndpointh, 0180026c

/*
C++: CMGetLastDevice(TConnectedDevice*)
MPW-mangled: CMGetLastDevice__FP16TConnectedDevice
*/
    JT _Z15CMGetLastDeviceP16TConnectedDevice, 01800270

/*
C++: CMGetServiceVersion(unsigned long, unsigned long*)
MPW-mangled: CMGetServiceVersion__FUlPUl
*/
    JT _Z19CMGetServiceVersionmPm, 01800274

/*
C++: CMSCPGetLastLoadedPackage(unsigned long*, unsigned long*)
MPW-mangled: CMSCPGetLastLoadedPackage__FPUlT1
*/
    JT CMSCPGetLastLoadedPackage__FPUlT1, 01800278

/*
C++: CMSCPLoad(unsigned long, unsigned long, unsigned long)
MPW-mangled: CMSCPLoad__FUlN21
*/
    JT CMSCPLoad__FUlN21, 0180027c

/*
C++: CMSCPSetLastLoadedPackage(unsigned long, unsigned long)
MPW-mangled: CMSCPSetLastLoadedPackage__FUlT1
*/
    JT CMSCPSetLastLoadedPackage__FUlT1, 01800280

/*
C++: CMSendMessage(TCMEvent*, unsigned long, TCMEvent*, unsigned long)
MPW-mangled: CMSendMessage__FP8TCMEventUlT1T2
*/
    JT CMSendMessage__FP8TCMEventUlT1T2, 01800284

/*
C++: CMSetLastDevice(TConnectedDevice*)
MPW-mangled: CMSetLastDevice__FP16TConnectedDevice
*/
    JT _Z15CMSetLastDeviceP16TConnectedDevice, 01800288

/*
C++: CMStartServiceInternal(TOptionArray*, TServiceInfo*)
MPW-mangled: CMStartServiceInternal__FP12TOptionArrayP12TServiceInfo
*/
    JT _Z22CMStartServiceInternalP12TOptionArrayP12TServiceInfo, 01800290

/*
C++: CMStartService(TOptionArray*, TServiceInfo*)
MPW-mangled: CMStartService__FP12TOptionArrayP12TServiceInfo
*/
    JT _Z14CMStartServiceP12TOptionArrayP12TServiceInfo, 0180028c

/*
C++: TCommToolProtocol::CTAcceptComplete(long)
MPW-mangled: CTAcceptComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol16CTAcceptCompleteEl, 01800568

/*
C++: TCommToolProtocol::CTAcceptStart()
MPW-mangled: CTAcceptStart__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol13CTAcceptStartEv, 0180056c

/*
C++: TCommToolProtocol::CTAddCurrentOptions(TOptionArray*)
MPW-mangled: CTAddCurrentOptions__17TCommToolProtocolFP12TOptionArray
*/
    JT _ZN17TCommToolProtocol19CTAddCurrentOptionsEP12TOptionArray, 01800570

/*
C++: TCommToolProtocol::CTAddDefaultOptions(TOptionArray*)
MPW-mangled: CTAddDefaultOptions__17TCommToolProtocolFP12TOptionArray
*/
    JT _ZN17TCommToolProtocol19CTAddDefaultOptionsEP12TOptionArray, 01800574

/*
C++: TCommToolProtocol::CTAllowAbort()
MPW-mangled: CTAllowAbort__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol12CTAllowAbortEv, 01800578

/*
C++: TCommToolProtocol::CTBindComplete(long)
MPW-mangled: CTBindComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol14CTBindCompleteEl, 018024ac

/*
C++: TCommToolProtocol::CTBind()
MPW-mangled: CTBind__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol6CTBindEv, 0180057c

/*
C++: TCommToolProtocol::CTChannelNumberToRequestType(CommToolChannelNumber)
MPW-mangled: CTChannelNumberToRequestType__17TCommToolProtocolF21CommToolChannelNumber
*/
    JT _ZN17TCommToolProtocol28CTChannelNumberToRequestTypeE21CommToolChannelNumber, 01800580

/*
C++: TCommToolProtocol::CTCloseComplete(long)
MPW-mangled: CTCloseComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol15CTCloseCompleteEl, 01800588

/*
C++: TCommToolProtocol::CTClose()
MPW-mangled: CTClose__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol7CTCloseEv, 01800584

/*
C++: TCommToolProtocol::CTCompleteRequest(CommToolChannelNumber, long)
MPW-mangled: CTCompleteRequest__17TCommToolProtocolF21CommToolChannelNumberl
*/
    JT _ZN17TCommToolProtocol17CTCompleteRequestE21CommToolChannelNumberl, 0180058c

/*
C++: TCommToolProtocol::CTCompleteRequest(CommToolChannelNumber, long, TCommToolReply&)
MPW-mangled: CTCompleteRequest__17TCommToolProtocolF21CommToolChannelNumberlR14TCommToolReply
*/
    JT _ZN17TCommToolProtocol17CTCompleteRequestE21CommToolChannelNumberlR14TCommToolReply, 01800590

/*
C++: TCommToolProtocol::CTCompleteRequest(TUMsgToken&, long)
MPW-mangled: CTCompleteRequest__17TCommToolProtocolFR10TUMsgTokenl
*/
    JT _ZN17TCommToolProtocol17CTCompleteRequestER10TUMsgTokenl, 01800594

/*
C++: TCommToolProtocol::CTCompleteRequest(TUMsgToken&, long, TCommToolReply&)
MPW-mangled: CTCompleteRequest__17TCommToolProtocolFR10TUMsgTokenlR14TCommToolReply
*/
    JT _ZN17TCommToolProtocol17CTCompleteRequestER10TUMsgTokenlR14TCommToolReply, 01800598

/*
C++: TCommToolProtocol::CTConnectComplete(long)
MPW-mangled: CTConnectComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol17CTConnectCompleteEl, 0180059c

/*
C++: TCommToolProtocol::CTConnectStart()
MPW-mangled: CTConnectStart__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol14CTConnectStartEv, 018005a0

/*
C++: TCommToolProtocol::CTDisconnectComplete(long)
MPW-mangled: CTDisconnectComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol20CTDisconnectCompleteEl, 018005a4

/*
C++: TCommToolProtocol::CTDoControl(unsigned long, unsigned long)
MPW-mangled: CTDoControl__17TCommToolProtocolFUlT1
*/
    JT CTDoControl__17TCommToolProtocolFUlT1, 018005a8

/*
C++: TCommToolProtocol::CTDoKillControl(unsigned long)
MPW-mangled: CTDoKillControl__17TCommToolProtocolFUl
*/
    JT _ZN17TCommToolProtocol15CTDoKillControlEm, 018005ac

/*
C++: TCommToolProtocol::CTDoKillGetCommEvent()
MPW-mangled: CTDoKillGetCommEvent__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol20CTDoKillGetCommEventEv, 018005b0

/*
C++: TCommToolProtocol::CTFlushChannel(CommToolRequestType, long)
MPW-mangled: CTFlushChannel__17TCommToolProtocolF19CommToolRequestTypel
*/
    JT _ZN17TCommToolProtocol14CTFlushChannelE19CommToolRequestTypel, 018005b4

/*
C++: TCommToolProtocol::CTGetAbortErr()
MPW-mangled: CTGetAbortErr__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol13CTGetAbortErrEv, 018005b8

/*
C++: TCommToolProtocol::CTGetCMOTransportInfo()
MPW-mangled: CTGetCMOTransportInfo__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol21CTGetCMOTransportInfoEv, 018005bc

/*
C++: TCommToolProtocol::CTGetCommEvent()
MPW-mangled: CTGetCommEvent__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol14CTGetCommEventEv, 018005c0

/*
C++: TCommToolProtocol::CTGetComplete(long, unsigned char, unsigned long)
MPW-mangled: CTGetComplete__17TCommToolProtocolFlUcUl
*/
    JT _ZN17TCommToolProtocol13CTGetCompleteElhm, 018005c4

/*
C++: TCommToolProtocol::CTGetConnectInfo()
MPW-mangled: CTGetConnectInfo__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol16CTGetConnectInfoEv, 018005c8

/*
C++: TCommToolProtocol::CTGetConnectParms()
MPW-mangled: CTGetConnectParms__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol17CTGetConnectParmsEv, 018005cc

/*
C++: TCommToolProtocol::CTGetControlOptionsInfo()
MPW-mangled: CTGetControlOptionsInfo__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol23CTGetControlOptionsInfoEv, 01802674

/*
C++: TCommToolProtocol::CTGetCurGetData()
MPW-mangled: CTGetCurGetData__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol15CTGetCurGetDataEv, 0180268c

/*
C++: TCommToolProtocol::CTGetCurGetFrameData()
MPW-mangled: CTGetCurGetFrameData__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol20CTGetCurGetFrameDataEv, 01802690

/*
C++: TCommToolProtocol::CTGetCurGetNonBlocking()
MPW-mangled: CTGetCurGetNonBlocking__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol22CTGetCurGetNonBlockingEv, 01802694

/*
C++: TCommToolProtocol::CTGetCurGetThreshold()
MPW-mangled: CTGetCurGetThreshold__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol20CTGetCurGetThresholdEv, 01802698

/*
C++: TCommToolProtocol::CTGetCurPutData()
MPW-mangled: CTGetCurPutData__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol15CTGetCurPutDataEv, 01802680

/*
C++: TCommToolProtocol::CTGetCurPutEndOfFrame()
MPW-mangled: CTGetCurPutEndOfFrame__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol21CTGetCurPutEndOfFrameEv, 01802688

/*
C++: TCommToolProtocol::CTGetCurPutFrameData()
MPW-mangled: CTGetCurPutFrameData__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol20CTGetCurPutFrameDataEv, 01802684

/*
C++: TCommToolProtocol::CTGetCurRequestOpCode()
MPW-mangled: CTGetCurRequestOpCode__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol21CTGetCurRequestOpCodeEv, 018026b4

/*
C++: TCommToolProtocol::CTGetGetBytesOptionsInfo()
MPW-mangled: CTGetGetBytesOptionsInfo__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol24CTGetGetBytesOptionsInfoEv, 01802678

/*
C++: TCommToolProtocol::CTGetPassiveClaim()
MPW-mangled: CTGetPassiveClaim__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol17CTGetPassiveClaimEv, 0180269c

/*
C++: TCommToolProtocol::CTGetPassiveState()
MPW-mangled: CTGetPassiveState__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol17CTGetPassiveStateEv, 018026a4

/*
C++: TCommToolProtocol::CTGetProtAddr()
MPW-mangled: CTGetProtAddr__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol13CTGetProtAddrEv, 018005d0

/*
C++: TCommToolProtocol::CTGetPutBytesOptionsInfo()
MPW-mangled: CTGetPutBytesOptionsInfo__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol24CTGetPutBytesOptionsInfoEv, 0180267c

/*
C++: TCommToolProtocol::CTGetReceiveMessageBufSize()
MPW-mangled: CTGetReceiveMessageBufSize__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol26CTGetReceiveMessageBufSizeEv, 018026b8

/*
C++: TCommToolProtocol::CTGetRequestListItem(unsigned char)
MPW-mangled: CTGetRequestListItem__17TCommToolProtocolFUc
*/
    JT _ZN17TCommToolProtocol20CTGetRequestListItemEh, 018005d8

/*
C++: TCommToolProtocol::CTGetRequestSize()
MPW-mangled: CTGetRequestSize__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol16CTGetRequestSizeEv, 018005dc

/*
C++: TCommToolProtocol::CTGetRequest()
MPW-mangled: CTGetRequest__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol12CTGetRequestEv, 018005d4

/*
C++: TCommToolProtocol::CTGetRequestsToKill()
MPW-mangled: CTGetRequestsToKill__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol19CTGetRequestsToKillEv, 018005e0

/*
C++: TCommToolProtocol::CTGetTerminationEvent()
MPW-mangled: CTGetTerminationEvent__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol21CTGetTerminationEventEv, 018005e4

/*
C++: TCommToolProtocol::CTGetToolConnectState()
MPW-mangled: CTGetToolConnectState__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol21CTGetToolConnectStateEv, 018005e8

/*
C++: TCommToolProtocol::CTGetToolId()
MPW-mangled: CTGetToolId__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol11CTGetToolIdEv, 018005ec

/*
C++: TCommToolProtocol::CTGetToolPort(unsigned long, TUPort&)
MPW-mangled: CTGetToolPort__17TCommToolProtocolFUlR6TUPort
*/
    JT _ZN17TCommToolProtocol13CTGetToolPortEmR6TUPort, 018005f0

/*
C++: TCommToolProtocol::CTGetToolPort()
MPW-mangled: CTGetToolPort__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol13CTGetToolPortEv, 01802670

/*
C++: TCommToolProtocol::CTGetWaitingForResNotify()
MPW-mangled: CTGetWaitingForResNotify__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol24CTGetWaitingForResNotifyEv, 018026ac

/*
C++: TCommToolProtocol::CTHandleInternalEvent()
MPW-mangled: CTHandleInternalEvent__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol21CTHandleInternalEventEv, 0180278c

/*
C++: TCommToolProtocol::CTHandleReply(unsigned long, unsigned long)
MPW-mangled: CTHandleReply__17TCommToolProtocolFUlT1
*/
    JT CTHandleReply__17TCommToolProtocolFUlT1, 018005f4

/*
C++: TCommToolProtocol::CTHandleRequest(TUMsgToken&, unsigned long)
MPW-mangled: CTHandleRequest__17TCommToolProtocolFR10TUMsgTokenUl
*/
    JT _ZN17TCommToolProtocol15CTHandleRequestER10TUMsgTokenm, 018005f8

/*
C++: TCommToolProtocol::CTHoldAbort()
MPW-mangled: CTHoldAbort__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol11CTHoldAbortEv, 018005fc

/*
C++: TCommToolProtocol::CTInitAsyncRPCMsg(TUAsyncMessage&, unsigned long)
MPW-mangled: CTInitAsyncRPCMsg__17TCommToolProtocolFR14TUAsyncMessageUl
*/
    JT _ZN17TCommToolProtocol17CTInitAsyncRPCMsgER14TUAsyncMessagem, 01800600

/*
C++: TCommToolProtocol::CTKillGetComplete(long)
MPW-mangled: CTKillGetComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol17CTKillGetCompleteEl, 01800604

/*
C++: TCommToolProtocol::CTKillPutComplete(long)
MPW-mangled: CTKillPutComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol17CTKillPutCompleteEl, 01800608

/*
C++: TCommToolProtocol::CTKillRequestComplete(CommToolRequestType, long)
MPW-mangled: CTKillRequestComplete__17TCommToolProtocolF19CommToolRequestTypel
*/
    JT _ZN17TCommToolProtocol21CTKillRequestCompleteE19CommToolRequestTypel, 0180060c

/*
C++: TCommToolProtocol::CTListenComplete(long)
MPW-mangled: CTListenComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol16CTListenCompleteEl, 01800610

/*
C++: TCommToolProtocol::CTListenStart()
MPW-mangled: CTListenStart__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol13CTListenStartEv, 01800614

/*
C++: TCommToolProtocol::CTOpenComplete()
MPW-mangled: CTOpenComplete__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol14CTOpenCompleteEv, 01800618

/*
C++: TCommToolProtocol::CTOpenContinue()
MPW-mangled: CTOpenContinue__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol14CTOpenContinueEv, 0180061c

/*
C++: TCommToolProtocol::CTOpenStart(TOptionArray*)
MPW-mangled: CTOpenStart__17TCommToolProtocolFP12TOptionArray
*/
    JT _ZN17TCommToolProtocol11CTOpenStartEP12TOptionArray, 01800620

/*
C++: TCommToolProtocol::CTPostCommEvent(TCommToolGetEventReply&, long)
MPW-mangled: CTPostCommEvent__17TCommToolProtocolFR22TCommToolGetEventReplyl
*/
    JT _ZN17TCommToolProtocol15CTPostCommEventER22TCommToolGetEventReplyl, 01800624

/*
C++: TCommToolProtocol::CTProcessGetBytesOptionComplete(unsigned long)
MPW-mangled: CTProcessGetBytesOptionComplete__17TCommToolProtocolFUl
*/
    JT _ZN17TCommToolProtocol31CTProcessGetBytesOptionCompleteEm, 018024c8

/*
C++: TCommToolProtocol::CTProcessGetBytesOptionStart(TOption*, unsigned long, unsigned long)
MPW-mangled: CTProcessGetBytesOptionStart__17TCommToolProtocolFP7TOptionUlT2
*/
    JT CTProcessGetBytesOptionStart__17TCommToolProtocolFP7TOptionUlT2, 018024c0

/*
C++: TCommToolProtocol::CTProcessOptionComplete(unsigned long)
MPW-mangled: CTProcessOptionComplete__17TCommToolProtocolFUl
*/
    JT _ZN17TCommToolProtocol23CTProcessOptionCompleteEm, 018024e4

/*
C++: TCommToolProtocol::CTProcessOptionStart(TOption*, unsigned long, unsigned long)
MPW-mangled: CTProcessOptionStart__17TCommToolProtocolFP7TOptionUlT2
*/
    JT CTProcessOptionStart__17TCommToolProtocolFP7TOptionUlT2, 018024b4

/*
C++: TCommToolProtocol::CTProcessOption(TOption*, unsigned long, unsigned long)
MPW-mangled: CTProcessOption__17TCommToolProtocolFP7TOptionUlT2
*/
    JT CTProcessOption__17TCommToolProtocolFP7TOptionUlT2, 01800628

/*
C++: TCommToolProtocol::CTProcessPutBytesOptionComplete(unsigned long)
MPW-mangled: CTProcessPutBytesOptionComplete__17TCommToolProtocolFUl
*/
    JT _ZN17TCommToolProtocol31CTProcessPutBytesOptionCompleteEm, 018024c4

/*
C++: TCommToolProtocol::CTProcessPutBytesOptionStart(TOption*, unsigned long, unsigned long)
MPW-mangled: CTProcessPutBytesOptionStart__17TCommToolProtocolFP7TOptionUlT2
*/
    JT CTProcessPutBytesOptionStart__17TCommToolProtocolFP7TOptionUlT2, 018024bc

/*
C++: TCommToolProtocol::CTPutComplete(long, unsigned long)
MPW-mangled: CTPutComplete__17TCommToolProtocolFlUl
*/
    JT _ZN17TCommToolProtocol13CTPutCompleteElm, 0180062c

/*
C++: TCommToolProtocol::CTReleaseComplete(long)
MPW-mangled: CTReleaseComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol17CTReleaseCompleteEl, 01800630

/*
C++: TCommToolProtocol::CTReleaseStart()
MPW-mangled: CTReleaseStart__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol14CTReleaseStartEv, 01800634

/*
C++: TCommToolProtocol::CTRequestTypeToChannelNumber(CommToolRequestType)
MPW-mangled: CTRequestTypeToChannelNumber__17TCommToolProtocolF19CommToolRequestType
*/
    JT _ZN17TCommToolProtocol28CTRequestTypeToChannelNumberE19CommToolRequestType, 01800638

/*
C++: TCommToolProtocol::CTResArbClaimNotification(unsigned char*, unsigned char*)
MPW-mangled: CTResArbClaimNotification__17TCommToolProtocolFPUcT1
*/
    JT CTResArbClaimNotification__17TCommToolProtocolFPUcT1, 018024e0

/*
C++: TCommToolProtocol::CTResArbReleaseComplete(long)
MPW-mangled: CTResArbReleaseComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol23CTResArbReleaseCompleteEl, 018024dc

/*
C++: TCommToolProtocol::CTResArbReleaseStart(unsigned char*, unsigned char*)
MPW-mangled: CTResArbReleaseStart__17TCommToolProtocolFPUcT1
*/
    JT CTResArbReleaseStart__17TCommToolProtocolFPUcT1, 018024d8

/*
C++: TCommToolProtocol::CTResArbRelease(unsigned char*, unsigned char*)
MPW-mangled: CTResArbRelease__17TCommToolProtocolFPUcT1
*/
    JT CTResArbRelease__17TCommToolProtocolFPUcT1, 018024d4

/*
C++: TCommToolProtocol::CTSetAbortErr(long)
MPW-mangled: CTSetAbortErr__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol13CTSetAbortErrEl, 0180063c

/*
C++: TCommToolProtocol::CTSetCMOTransportInfo(TCMOTransportInfo&)
MPW-mangled: CTSetCMOTransportInfo__17TCommToolProtocolFR17TCMOTransportInfo
*/
    JT _ZN17TCommToolProtocol21CTSetCMOTransportInfoER17TCMOTransportInfo, 01800644

/*
C++: TCommToolProtocol::CTSetChannelFilter(CommToolRequestType, unsigned char)
MPW-mangled: CTSetChannelFilter__17TCommToolProtocolF19CommToolRequestTypeUc
*/
    JT _ZN17TCommToolProtocol18CTSetChannelFilterE19CommToolRequestTypeh, 01800640

/*
C++: TCommToolProtocol::CTSetConnectInfo(TCMOCTConnectInfo&)
MPW-mangled: CTSetConnectInfo__17TCommToolProtocolFR17TCMOCTConnectInfo
*/
    JT _ZN17TCommToolProtocol16CTSetConnectInfoER17TCMOCTConnectInfo, 01800648

/*
C++: TCommToolProtocol::CTSetPassiveClaim(unsigned char)
MPW-mangled: CTSetPassiveClaim__17TCommToolProtocolFUc
*/
    JT _ZN17TCommToolProtocol17CTSetPassiveClaimEh, 018026a0

/*
C++: TCommToolProtocol::CTSetPassiveState(unsigned char)
MPW-mangled: CTSetPassiveState__17TCommToolProtocolFUc
*/
    JT _ZN17TCommToolProtocol17CTSetPassiveStateEh, 018026a8

/*
C++: TCommToolProtocol::CTSetRequestsToKill(CommToolRequestType)
MPW-mangled: CTSetRequestsToKill__17TCommToolProtocolF19CommToolRequestType
*/
    JT _ZN17TCommToolProtocol19CTSetRequestsToKillE19CommToolRequestType, 0180064c

/*
C++: TCommToolProtocol::CTSetTerminationEvent(unsigned long)
MPW-mangled: CTSetTerminationEvent__17TCommToolProtocolFUl
*/
    JT _ZN17TCommToolProtocol21CTSetTerminationEventEm, 01800650

/*
C++: TCommToolProtocol::CTSetToolConnectState(unsigned long)
MPW-mangled: CTSetToolConnectState__17TCommToolProtocolFUl
*/
    JT _ZN17TCommToolProtocol21CTSetToolConnectStateEm, 01800654

/*
C++: TCommToolProtocol::CTSetWaitingForResNotify(unsigned char)
MPW-mangled: CTSetWaitingForResNotify__17TCommToolProtocolFUc
*/
    JT _ZN17TCommToolProtocol24CTSetWaitingForResNotifyEh, 018026b0

/*
C++: TCommToolProtocol::CTShouldAbort(unsigned long, long)
MPW-mangled: CTShouldAbort__17TCommToolProtocolFUll
*/
    JT _ZN17TCommToolProtocol13CTShouldAbortEml, 01800658

/*
C++: TCommToolProtocol::CTStartAbort(long)
MPW-mangled: CTStartAbort__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol12CTStartAbortEl, 0180065c

/*
C++: TCommToolProtocol::CTTaskConstructor()
MPW-mangled: CTTaskConstructor__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol17CTTaskConstructorEv, 01800660

/*
C++: TCommToolProtocol::CTTaskDestructor()
MPW-mangled: CTTaskDestructor__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol16CTTaskDestructorEv, 01800664

/*
C++: TCommToolProtocol::CTTerminateComplete()
MPW-mangled: CTTerminateComplete__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol19CTTerminateCompleteEv, 01800668

/*
C++: TCommToolProtocol::CTTerminateConnection()
MPW-mangled: CTTerminateConnection__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol21CTTerminateConnectionEv, 0180066c

/*
C++: TCommToolProtocol::CTUnbindComplete(long)
MPW-mangled: CTUnbindComplete__17TCommToolProtocolFl
*/
    JT _ZN17TCommToolProtocol16CTUnbindCompleteEl, 018024b0

/*
C++: TCommToolProtocol::CTUnbind()
MPW-mangled: CTUnbind__17TCommToolProtocolFv
*/
    JT _ZN17TCommToolProtocol8CTUnbindEv, 01800670

/*
C++: CallHandleProblem(PrintConnect*, TPrinter*, long, unsigned long, unsigned char)
MPW-mangled: CallHandleProblem__FP12PrintConnectP8TPrinterlUlUc
*/
    JT _Z17CallHandleProblemP12PrintConnectP8TPrinterlmh, 01801614

/*
C++: TEndpointEventHandler::CallServiceBlocking(unsigned long, TAEvent*, long, TAEvent*, long, unsigned long)
MPW-mangled: CallServiceBlocking__21TEndpointEventHandlerFUlP7TAEventlT2T3T1
*/
    JT CallServiceBlocking__21TEndpointEventHandlerFUlP7TAEventlT2T3T1, 01800264

/*
C++: TEndpointEventHandler::CallService(unsigned long, TUAsyncMessage*, TAEvent*, long, TAEvent*, long, unsigned long, unsigned char)
MPW-mangled: CallService__21TEndpointEventHandlerFUlP14TUAsyncMessageP7TAEventlT3T4T1Uc
*/
    JT CallService__21TEndpointEventHandlerFUlP14TUAsyncMessageP7TAEventlT3T4T1Uc, 01800260

/*
C++: TInterpreter::Call(const RefVar&, long)
MPW-mangled: Call__12TInterpreterFRC6RefVarl
*/
    JT _ZN12TInterpreter4CallERK6RefVarl, 01801ec8

/*
C++: TUSoundChannel::Cancel(unsigned long)
MPW-mangled: Cancel__14TUSoundChannelFUl
*/
    JT _ZN14TUSoundChannel6CancelEm, 01802c28

/*
C++: CardAttrMemReadByte(void*)
MPW-mangled: CardAttrMemReadByte__FPv
*/
    JT _Z19CardAttrMemReadBytePv, 01800ea0

/*
C++: CardAttrMemReadDelay()
MPW-mangled: CardAttrMemReadDelay__Fv
*/
    JT _Z20CardAttrMemReadDelayv, 01800ea4

/*
C++: CardAttrMemWriteByte(void*, unsigned char)
MPW-mangled: CardAttrMemWriteByte__FPvUc
*/
    JT _Z20CardAttrMemWriteBytePvh, 01800ea8

/*
C++: CardAttrMemWriteDelay()
MPW-mangled: CardAttrMemWriteDelay__Fv
*/
    JT _Z21CardAttrMemWriteDelayv, 01800eac

/*
C++: TCardHandler::CardSpecific(unsigned long, void*, unsigned long)
MPW-mangled: CardSpecific__12TCardHandlerFUlPvT1
*/
    JT CardSpecific__12TCardHandlerFUlPvT1, 01800eb0

/*
C++: TUMsgToken::CashMessageToken(unsigned long*, void*, unsigned long, unsigned long, unsigned char)
MPW-mangled: CashMessageToken__10TUMsgTokenFPUlPvUlT3Uc
*/
    JT CashMessageToken__10TUMsgTokenFPUlPvUlT3Uc, 01800eb4

/*
C++: ChangeInterruptPriority(InterruptObject*, InterruptPriority)
MPW-mangled: ChangeInterruptPriority__FP15InterruptObject17InterruptPriority
*/
    JT _Z23ChangeInterruptPriorityP15InterruptObject17InterruptPriority, 01802bc4

/*
C++: TUPhys::ChangeVirtualMapping(unsigned long, unsigned long, EPhysChangeType)
MPW-mangled: ChangeVirtualMapping__6TUPhysFUlT115EPhysChangeType
*/
    JT ChangeVirtualMapping__6TUPhysFUlT115EPhysChangeType, 01802540

/*
C++: TView::Changed(const RefVar&)
MPW-mangled: Changed__5TViewFRC6RefVar
*/
    JT _ZN5TView7ChangedERK6RefVar, 01800b78

/*
C++: TView::Changed(const RefVar&, const RefVar&)
MPW-mangled: Changed__5TViewFRC6RefVarT1
*/
    JT Changed__5TViewFRC6RefVarT1, 01800b7c

/*
C++: CharacterPosition(const RefVar&, unsigned short, long)
MPW-mangled: CharacterPosition__FRC6RefVarUsl
*/
    JT _Z17CharacterPositionRK6RefVartl, 01802bb4

/*
C++: TDoubleQContainer::CheckBeforeAdd(void*)
MPW-mangled: CheckBeforeAdd__17TDoubleQContainerFPv
*/
    JT _ZN17TDoubleQContainer14CheckBeforeAddEPv, 01800eb8
    JT CheckHeap, 01801624

/*
C++: static TURealTimeAlarm::CheckIn(unsigned long)
MPW-mangled: CheckIn__15TURealTimeAlarmSFUl
*/
    JT _ZN15TURealTimeAlarm7CheckInEm, 01800ebc

/*
C++: static TURealTimeAlarm::CheckOut(unsigned long)
MPW-mangled: CheckOut__15TURealTimeAlarmSFUl
*/
    JT _ZN15TURealTimeAlarm8CheckOutEm, 01800ec0

/*
C++: TATA::CheckPowerMode(unsigned char*, unsigned char)
MPW-mangled: CheckPowerMode__4TATAFPUcUc
*/
    JT _ZN4TATA14CheckPowerModeEPhh, 01802a68

/*
C++: TCardATALoader::ChecksumOf(unsigned char*, unsigned long)
MPW-mangled: ChecksumOf__14TCardATALoaderFPUcUl
*/
    JT _ZN14TCardATALoader10ChecksumOfEPhm, 01802a6c

/*
C++: TView::ChildBoundsChanged(TView*, TRect&)
MPW-mangled: ChildBoundsChanged__5TViewFP5TViewR5TRect
*/
    JT _ZN5TView18ChildBoundsChangedEP5TViewR5TRect, 01800b84

/*
C++: TView::ChildViewFrames()
MPW-mangled: ChildViewFrames__5TViewFv
*/
    JT _ZN5TView15ChildViewFramesEv, 01800b90

/*
C++: TView::ChildViewMoved(TView*, TPoint)
MPW-mangled: ChildViewMoved__5TViewFP5TView6TPoint
*/
    JT _ZN5TView14ChildViewMovedEP5TView6TPoint, 01800b94

/*
C++: TView::ChildrenHeight(long*)
MPW-mangled: ChildrenHeight__5TViewFPl
*/
    JT _ZN5TView14ChildrenHeightEPl, 01800b8c

/*
C++: TView::Children()
MPW-mangled: Children__5TViewFv
*/
    JT _ZN5TView8ChildrenEv, 01800b88

/*
C++: ClassInfoByName(const char*, const char*, unsigned long)
MPW-mangled: ClassInfoByName__FPCcT1Ul
*/
    JT _Z15ClassInfoByNamePKcS0_m, 01800ec8

/*
C++: ClassInfoFromHunkByName(void*, const char*, const char*)
MPW-mangled: ClassInfoFromHunkByName__FPvPCcT2
*/
    JT ClassInfoFromHunkByName__FPvPCcT2, 01800ecc

/*
C++: TProtocol::ClassInfo() const
MPW-mangled: ClassInfo__9TProtocolCFv
*/
    JT ClassInfo__9TProtocolCFv, 01800ec4

/*
C++: ClassOf(const RefVar&)
MPW-mangled: ClassOf__FRC6RefVar
*/
    JT _Z7ClassOfRK6RefVar, 01800834

/*
C++: TDate::CleanUpFields()
MPW-mangled: CleanUpFields__5TDateFv
*/
    JT _ZN5TDate13CleanUpFieldsEv, 0180260c

/*
C++: static TRealTimeClock::Cleanup()
MPW-mangled: Cleanup__14TRealTimeClockSFv
*/
    JT _ZN14TRealTimeClock7CleanupEv, 01800ed0

/*
C++: static TRealTimeClock::ClearAlarm(unsigned long)
MPW-mangled: ClearAlarm__14TRealTimeClockSFUl
*/
    JT _ZN14TRealTimeClock10ClearAlarmEm, 01800ee4

/*
C++: static TURealTimeAlarm::ClearAlarm(unsigned long)
MPW-mangled: ClearAlarm__15TURealTimeAlarmSFUl
*/
    JT _ZN15TURealTimeAlarm10ClearAlarmEm, 01800ee8
    JT ClearAllInterrupts, 01800eec

/*
C++: TCardPCMCIA::ClearFields()
MPW-mangled: ClearFields__11TCardPCMCIAFv
*/
    JT _ZN11TCardPCMCIA11ClearFieldsEv, 01800ef0

/*
C++: TAppWorld::ClearFilter(unsigned long)
MPW-mangled: ClearFilter__9TAppWorldFUl
*/
    JT _ZN9TAppWorld11ClearFilterEm, 01801a28

/*
C++: TView::ClearFlags(unsigned long)
MPW-mangled: ClearFlags__5TViewFUl
*/
    JT _ZN5TView10ClearFlagsEm, 01800b98
    JT ClearInterrupt, 01802528

/*
C++: ClearRefHandles()
MPW-mangled: ClearRefHandles__Fv
*/
    JT _Z15ClearRefHandlesv, 01802498

/*
C++: TCardSocket::ClearSocketInterrupt(TSocketInt)
MPW-mangled: ClearSocketInterrupt__11TCardSocketF10TSocketInt
*/
    JT _ZN11TCardSocket20ClearSocketInterruptE10TSocketInt, 01800ef4

/*
C++: TCardPCMCIA::Clear()
MPW-mangled: Clear__11TCardPCMCIAFv
*/
    JT _ZN11TCardPCMCIA5ClearEv, 01800ed4

/*
C++: TCardMessage::Clear()
MPW-mangled: Clear__12TCardMessageFv
*/
    JT _ZN12TCardMessage5ClearEv, 01800ed8

/*
C++: TCardLongLink::Clear()
MPW-mangled: Clear__13TCardLongLinkFv
*/
    JT _ZN13TCardLongLink5ClearEv, 01800edc

/*
C++: TATAPartitionInfo::Clear()
MPW-mangled: Clear__17TATAPartitionInfoFv
*/
    JT _ZN17TATAPartitionInfo5ClearEv, 01802510

/*
C++: TCardConfiguration::Clear()
MPW-mangled: Clear__18TCardConfigurationFv
*/
    JT _ZN18TCardConfiguration5ClearEv, 01800ee0
    JT Clibrary_disposePtr, 01801780
    JT Clibrary_ex_spt, 01800040

/*
C++: TView::Clipper() const
MPW-mangled: Clipper__5TViewCFv
*/
    JT Clipper__5TViewCFv, 01800ba0

/*
C++: Clone(const RefVar&)
MPW-mangled: Clone__FRC6RefVar
*/
    JT _Z5CloneRK6RefVar, 01800838

/*
C++: CloseAppleTalk(unsigned long)
MPW-mangled: CloseAppleTalk__FUl
*/
    JT _Z14CloseAppleTalkm, 018020a8

/*
C++: TCommTool::CloseComplete(long)
MPW-mangled: CloseComplete__9TCommToolFl
*/
    JT _ZN9TCommTool13CloseCompleteEl, 01800538

/*
C++: TATPInterface::CloseResponder(unsigned char)
MPW-mangled: CloseResponder__13TATPInterfaceFUc
*/
    JT _ZN13TATPInterface14CloseResponderEh, 0180209c

/*
C++: TUSoundChannel::Close()
MPW-mangled: Close__14TUSoundChannelFv
*/
    JT _ZN14TUSoundChannel5CloseEv, 01802c2c

/*
C++: CPartPipe::Close()
MPW-mangled: Close__9CPartPipeFv
*/
    JT _ZN9CPartPipe5CloseEv, 01800ef8

/*
C++: TCommTool::Close()
MPW-mangled: Close__9TCommToolFv
*/
    JT _ZN9TCommTool5CloseEv, 01800534

/*
C++: TEndpoint::Close()
MPW-mangled: Close__9TEndpointFv
*/
    JT _ZN9TEndpoint5CloseEv, 01800268

/*
C++: CoerceToDouble(const RefVar&)
MPW-mangled: CoerceToDouble__FRC6RefVar
*/
    JT _Z14CoerceToDoubleRK6RefVar, 0180083c

/*
C++: CoerceToInt(const RefVar&)
MPW-mangled: CoerceToInt__FRC6RefVar
*/
    JT _Z11CoerceToIntRK6RefVar, 01800840

/*
C++: TCardSocket::CommonMemBaseAddr()
MPW-mangled: CommonMemBaseAddr__11TCardSocketFv
*/
    JT _ZN11TCardSocket17CommonMemBaseAddrEv, 01800efc
    JT CompAdd, 01801750
    JT CompCompare, 01801754
    JT CompDiv, 01801758
    JT CompMul, 0180175c
    JT CompShift, 01801760
    JT CompSquareRoot, 01801764
    JT CompSub, 01801768

/*
C++: CompareStringNoCase(unsigned short*, unsigned short*)
MPW-mangled: CompareStringNoCase__FPUsT1
*/
    JT CompareStringNoCase__FPUsT1, 01801a2c

/*
C++: CompareTextNoCase(unsigned short*, long, unsigned short*, long)
MPW-mangled: CompareTextNoCase__FPUslT1T2
*/
    JT CompareTextNoCase__FPUslT1T2, 01801a30

/*
C++: TCompiler::Compile()
MPW-mangled: Compile__9TCompilerFv
*/
    JT _ZN9TCompiler7CompileEv, 01800844

/*
C++: TCommTool::CompleteRequest(CommToolChannelNumber, long)
MPW-mangled: CompleteRequest__9TCommToolF21CommToolChannelNumberl
*/
    JT _ZN9TCommTool15CompleteRequestE21CommToolChannelNumberl, 0180053c

/*
C++: TCommTool::CompleteRequest(CommToolChannelNumber, long, TCommToolReply&)
MPW-mangled: CompleteRequest__9TCommToolF21CommToolChannelNumberlR14TCommToolReply
*/
    JT _ZN9TCommTool15CompleteRequestE21CommToolChannelNumberlR14TCommToolReply, 01800540

/*
C++: TCommTool::CompleteRequest(TUMsgToken&, long)
MPW-mangled: CompleteRequest__9TCommToolFR10TUMsgTokenl
*/
    JT _ZN9TCommTool15CompleteRequestER10TUMsgTokenl, 01800544

/*
C++: TCommTool::CompleteRequest(TUMsgToken&, long, TCommToolReply&)
MPW-mangled: CompleteRequest__9TCommToolFR10TUMsgTokenlR14TCommToolReply
*/
    JT _ZN9TCommTool15CompleteRequestER10TUMsgTokenlR14TCommToolReply, 01800548

/*
C++: TCompressor::Compress(char**)
MPW-mangled: Compress__11TCompressorFPPc
*/
    JT _ZN11TCompressor8CompressEPPc, 01801a34

/*
C++: TCRC16::ComputeCRC(CBufferList&)
MPW-mangled: ComputeCRC__6TCRC16FR11CBufferList
*/
    JT _ZN6TCRC1610ComputeCRCER11CBufferList, 0180054c

/*
C++: CRingBuffer::ComputeGetVectors(unsigned char*&, long&, unsigned char*&, long&) const
MPW-mangled: ComputeGetVectors__11CRingBufferCFRPUcRlT1T2
*/
    JT ComputeGetVectors__11CRingBufferCFRPUcRlT1T2, 01801a38

/*
C++: CShadowRingBuffer::ComputeGetVectors(unsigned char*&, long&, unsigned char*&, long&) const
MPW-mangled: ComputeGetVectors__17CShadowRingBufferCFRPUcRlT1T2
*/
    JT ComputeGetVectors__17CShadowRingBufferCFRPUcRlT1T2, 01801a3c

/*
C++: CRingBuffer::ComputePutVectors(unsigned char*&, long&, unsigned char*&, long&) const
MPW-mangled: ComputePutVectors__11CRingBufferCFRPUcRlT1T2
*/
    JT ComputePutVectors__11CRingBufferCFRPUcRlT1T2, 01801a40

/*
C++: CShadowRingBuffer::ComputePutVectors(unsigned char*&, long&, unsigned char*&, long&) const
MPW-mangled: ComputePutVectors__17CShadowRingBufferCFRPUcRlT1T2
*/
    JT ComputePutVectors__17CShadowRingBufferCFRPUcRlT1T2, 01801a44

/*
C++: CShadowRingBuffer::ComputeTempGetVectors(unsigned long&, long&, unsigned long&, long&) const
MPW-mangled: ComputeTempGetVectors__17CShadowRingBufferCFRUlRlT1T2
*/
    JT ComputeTempGetVectors__17CShadowRingBufferCFRUlRlT1T2, 01801a48

/*
C++: TCommTool::ConnectCheck()
MPW-mangled: ConnectCheck__9TCommToolFv
*/
    JT _ZN9TCommTool12ConnectCheckEv, 01800554

/*
C++: TEndpointClient::ConnectComplete(TEndpointEvent*)
MPW-mangled: ConnectComplete__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient15ConnectCompleteEP14TEndpointEvent, 01800298

/*
C++: TCommTool::ConnectComplete(long)
MPW-mangled: ConnectComplete__9TCommToolFl
*/
    JT _ZN9TCommTool15ConnectCompleteEl, 01800558

/*
C++: TCommTool::ConnectStart()
MPW-mangled: ConnectStart__9TCommToolFv
*/
    JT _ZN9TCommTool12ConnectStartEv, 0180055c

/*
C++: TCommTool::Connect()
MPW-mangled: Connect__9TCommToolFv
*/
    JT _ZN9TCommTool7ConnectEv, 01800550

/*
C++: TEndpoint::Connect(TOptionArray*, TOptionArray*, CBufferSegment*, long*, unsigned long)
MPW-mangled: Connect__9TEndpointFP12TOptionArrayT1P14CBufferSegmentPlUl
*/
    JT Connect__9TEndpointFP12TOptionArrayT1P14CBufferSegmentPlUl, 01800294

/*
C++: TView::Constructor(const RefVar&, TView*)
MPW-mangled: Constructor__5TViewFRC6RefVarP5TView
*/
    JT _ZN5TView11ConstructorERK6RefVarP5TView, 01800ba4

/*
C++: TInterpreter::ControlPosition()
MPW-mangled: ControlPosition__12TInterpreterFv
*/
    JT _ZN12TInterpreter15ControlPositionEv, 0180200c

/*
C++: TDelayTimer::ConvertFromHardwareTime(unsigned long)
MPW-mangled: ConvertFromHardwareTime__11TDelayTimerFUl
*/
    JT _ZN11TDelayTimer23ConvertFromHardwareTimeEm, 018025a0

/*
C++: ConvertFromUnicode(const unsigned short*, void*, long, long)
MPW-mangled: ConvertFromUnicode__FPCUsPvlT3
*/
    JT ConvertFromUnicode__FPCUsPvlT3, 01801a4c

/*
C++: TDelayTimer::ConvertToHardwareTime(unsigned long)
MPW-mangled: ConvertToHardwareTime__11TDelayTimerFUl
*/
    JT _ZN11TDelayTimer21ConvertToHardwareTimeEm, 0180259c

/*
C++: ConvertToUnicode(const void*, unsigned short*, long, long)
MPW-mangled: ConvertToUnicode__FPCvPUslT3
*/
    JT _Z16ConvertToUnicodePKvPtll, 01801a50

/*
C++: TTime::ConvertTo(TimeUnits)
MPW-mangled: ConvertTo__5TTimeF9TimeUnits
*/
    JT _ZN5TTime9ConvertToE9TimeUnits, 01800f00

/*
C++: ConvertUnicodeChar(unsigned short*, char*, long)
MPW-mangled: ConvertUnicodeChar__FPUsPcl
*/
    JT _Z18ConvertUnicodeCharPtPcl, 01802b78

/*
C++: ConvertUnicodeCharacters(unsigned short*, char*, long, long)
MPW-mangled: ConvertUnicodeCharacters__FPUsPclT3
*/
    JT ConvertUnicodeCharacters__FPUsPclT3, 01802b74

/*
C++: TCommTool::CopyBackConnectPB(long)
MPW-mangled: CopyBackConnectPB__9TCommToolFl
*/
    JT _ZN9TCommTool17CopyBackConnectPBEl, 01800560

/*
C++: CopyBits(PixelMap*, PixelMap*, Rect*, Rect*, long, Region**)
MPW-mangled: CopyBits__FP8PixelMapT1P4RectT3lPP6Region
*/
    JT CopyBits__FP8PixelMapT1P4RectT3lPP6Region, 01801e6c
    JT CopyBytes, 01801628

/*
C++: TOption::CopyDataFrom(TOption*)
MPW-mangled: CopyDataFrom__7TOptionFP7TOption
*/
    JT _ZN7TOption12CopyDataFromEP7TOption, 0180029c

/*
C++: TUSharedMem::CopyFromShared(unsigned long*, void*, unsigned long, unsigned long, TUMsgToken*)
MPW-mangled: CopyFromShared__11TUSharedMemFPUlPvUlT3P10TUMsgToken
*/
    JT CopyFromShared__11TUSharedMemFPUlPvUlT3P10TUMsgToken, 01800f08

/*
C++: TOptionArray::CopyFromShared(unsigned long, unsigned long)
MPW-mangled: CopyFromShared__12TOptionArrayFUlT1
*/
    JT CopyFromShared__12TOptionArrayFUlT1, 018002a0

/*
C++: CMemObject::CopyFrom(unsigned long*, void*, unsigned long, unsigned long)
MPW-mangled: CopyFrom__10CMemObjectFPUlPvUlT3
*/
    JT CopyFrom__10CMemObjectFPUlPvUlT3, 01801a54
    JT CopyHandle, 0180176c

/*
C++: TCircleBuf::CopyIn(CBufferList*, unsigned long*)
MPW-mangled: CopyIn__10TCircleBufFP11CBufferListPUl
*/
    JT _ZN10TCircleBuf6CopyInEP11CBufferListPm, 01802a04

/*
C++: TCircleBuf::CopyIn(unsigned char*, unsigned long*, unsigned char, unsigned long)
MPW-mangled: CopyIn__10TCircleBufFPUcPUlUcUl
*/
    JT _ZN10TCircleBuf6CopyInEPhPmhm, 01802a08

/*
C++: CBufferList::CopyIn(const unsigned char*, long&)
MPW-mangled: CopyIn__11CBufferListFPCUcRl
*/
    JT _ZN11CBufferList6CopyInEPKhRl, 01801a58

/*
C++: CRingBuffer::CopyIn(CPipe*, long&)
MPW-mangled: CopyIn__11CRingBufferFP5CPipeRl
*/
    JT _ZN11CRingBuffer6CopyInEP5CPipeRl, 01801a5c

/*
C++: CRingBuffer::CopyIn(const unsigned char*, long&)
MPW-mangled: CopyIn__11CRingBufferFPCUcRl
*/
    JT _ZN11CRingBuffer6CopyInEPKhRl, 01801a60

/*
C++: CBufferSegment::CopyIn(const unsigned char*, long&)
MPW-mangled: CopyIn__14CBufferSegmentFPCUcRl
*/
    JT _ZN14CBufferSegment6CopyInEPKhRl, 01801a64

/*
C++: CShadowRingBuffer::CopyIn(const unsigned char*, long&)
MPW-mangled: CopyIn__17CShadowRingBufferFPCUcRl
*/
    JT _ZN17CShadowRingBuffer6CopyInEPKhRl, 01801a68

/*
C++: CShadowBufferSegment::CopyIn(const unsigned char*, long&)
MPW-mangled: CopyIn__20CShadowBufferSegmentFPCUcRl
*/
    JT _ZN20CShadowBufferSegment6CopyInEPKhRl, 01801a6c

/*
C++: static MemObjManager::CopyObject(MemObjType, void*, void*)
MPW-mangled: CopyObject__13MemObjManagerSF10MemObjTypePvT2
*/
    JT CopyObject__13MemObjManagerSF10MemObjTypePvT2, 01800f0c

/*
C++: TULockingSemaphore::CopyObject(unsigned long)
MPW-mangled: CopyObject__18TULockingSemaphoreFUl
*/
    JT _ZN18TULockingSemaphore10CopyObjectEm, 01800f10

/*
C++: TUObject::CopyObject(const unsigned long)
MPW-mangled: CopyObject__8TUObjectFCUl
*/
    JT _ZN8TUObject10CopyObjectEKm, 01800f14

/*
C++: TUMonitor::CopyObject(const TUMonitor&)
MPW-mangled: CopyObject__9TUMonitorFRC9TUMonitor
*/
    JT _ZN9TUMonitor10CopyObjectERK9TUMonitor, 01800f18

/*
C++: TUMonitor::CopyObject(unsigned long)
MPW-mangled: CopyObject__9TUMonitorFUl
*/
    JT _ZN9TUMonitor10CopyObjectEm, 01800f1c

/*
C++: TOptionArray::CopyOptionAt(long, TOption*)
MPW-mangled: CopyOptionAt__12TOptionArrayFlP7TOption
*/
    JT _ZN12TOptionArray12CopyOptionAtElP7TOption, 018002a4

/*
C++: TCircleBuf::CopyOut(CBufferList*, unsigned long*, unsigned long*)
MPW-mangled: CopyOut__10TCircleBufFP11CBufferListPUlT2
*/
    JT CopyOut__10TCircleBufFP11CBufferListPUlT2, 01802a0c

/*
C++: TCircleBuf::CopyOut(unsigned char*, unsigned long*, unsigned long*)
MPW-mangled: CopyOut__10TCircleBufFPUcPUlT2
*/
    JT CopyOut__10TCircleBufFPUcPUlT2, 01802a10

/*
C++: CBufferList::CopyOut(unsigned char*, long&)
MPW-mangled: CopyOut__11CBufferListFPUcRl
*/
    JT _ZN11CBufferList7CopyOutEPhRl, 01801a70

/*
C++: CRingBuffer::CopyOut(unsigned char*, long&)
MPW-mangled: CopyOut__11CRingBufferFPUcRl
*/
    JT _ZN11CRingBuffer7CopyOutEPhRl, 01801a74

/*
C++: CBufferSegment::CopyOut(unsigned char*, long&)
MPW-mangled: CopyOut__14CBufferSegmentFPUcRl
*/
    JT _ZN14CBufferSegment7CopyOutEPhRl, 01801a78

/*
C++: CShadowRingBuffer::CopyOut(unsigned char*, long&)
MPW-mangled: CopyOut__17CShadowRingBufferFPUcRl
*/
    JT _ZN17CShadowRingBuffer7CopyOutEPhRl, 01801a7c

/*
C++: CShadowBufferSegment::CopyOut(unsigned char*, long&)
MPW-mangled: CopyOut__20CShadowBufferSegmentFPUcRl
*/
    JT _ZN20CShadowBufferSegment7CopyOutEPhRl, 01801a80

/*
C++: TUDomainManager::CopyPhysPg(unsigned long, unsigned long, unsigned long)
MPW-mangled: CopyPhysPg__15TUDomainManagerFUlN21
*/
    JT CopyPhysPg__15TUDomainManagerFUlN21, 01800f20

/*
C++: TPackageIterator::CopyProtected()
MPW-mangled: CopyProtected__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator13CopyProtectedEv, 01800f24

/*
C++: TView::CopyProtection() const
MPW-mangled: CopyProtection__5TViewCFv
*/
    JT CopyProtection__5TViewCFv, 01800ba8

/*
C++: TUSharedMem::CopyToShared(void*, unsigned long, unsigned long, TUMsgToken*)
MPW-mangled: CopyToShared__11TUSharedMemFPvUlT2P10TUMsgToken
*/
    JT CopyToShared__11TUSharedMemFPvUlT2P10TUMsgToken, 01800f2c

/*
C++: TOptionArray::CopyToShared(unsigned long)
MPW-mangled: CopyToShared__12TOptionArrayFUl
*/
    JT _ZN12TOptionArray12CopyToSharedEm, 018002a8

/*
C++: CMemObject::CopyTo(void*, unsigned long, unsigned long)
MPW-mangled: CopyTo__10CMemObjectFPvUlT2
*/
    JT CopyTo__10CMemObjectFPvUlT2, 01801a84

/*
C++: TPartHandler::Copy(void*)
MPW-mangled: Copy__12TPartHandlerFPv
*/
    JT _ZN12TPartHandler4CopyEPv, 01800f04

/*
C++: TPackageIterator::Copyright()
MPW-mangled: Copyright__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator9CopyrightEv, 01800f28
    JT CountFreeBlocks, 0180162c
    JT CountHeapBlocks, 01801770

/*
C++: TCommTool::CreatePort(unsigned long, TUPort&)
MPW-mangled: CreatePort__9TCommToolFUlR6TUPort
*/
    JT _ZN9TCommTool10CreatePortEmR6TUPort, 01800564

/*
C++: TCardSocket::CreateSocketPhys(TUPhys*, unsigned long, unsigned long, unsigned char)
MPW-mangled: CreateSocketPhys__11TCardSocketFP6TUPhysUlT2Uc
*/
    JT CreateSocketPhys__11TCardSocketFP6TUPhysUlT2Uc, 01800f30

/*
C++: CreateTextStyleRecord(const RefVar&, StyleRecord*)
MPW-mangled: CreateTextStyleRecord__FRC6RefVarP11StyleRecord
*/
    JT _Z21CreateTextStyleRecordRK6RefVarP11StyleRecord, 01800bb0

/*
C++: CreateUniqueEnvironment(unsigned long, unsigned long*)
MPW-mangled: CreateUniqueEnvironment__FUlPUl
*/
    JT _Z23CreateUniqueEnvironmentmPm, 01802d8c

/*
C++: static TStoreHashTable::Create(TStore*)
MPW-mangled: Create__15TStoreHashTableSFP6TStore
*/
    JT _ZN15TStoreHashTable6CreateEP6TStore, 0180084c

/*
C++: CArrayIterator::CurrentIndex()
MPW-mangled: CurrentIndex__14CArrayIteratorFv
*/
    JT _ZN14CArrayIterator12CurrentIndexEv, 01801a88

/*
C++: TOptionIterator::CurrentIndex()
MPW-mangled: CurrentIndex__15TOptionIteratorFv
*/
    JT _ZN15TOptionIterator12CurrentIndexEv, 018002ac

/*
C++: CListIterator::CurrentItem()
MPW-mangled: CurrentItem__13CListIteratorFv
*/
    JT _ZN13CListIterator11CurrentItemEv, 01801a8c

/*
C++: TOptionIterator::CurrentOption()
MPW-mangled: CurrentOption__15TOptionIteratorFv
*/
    JT _ZN15TOptionIterator13CurrentOptionEv, 018002b0

/*
C++: CursorClone(const RefVar&)
MPW-mangled: CursorClone__FRC6RefVar
*/
    JT _Z11CursorCloneRK6RefVar, 01800854

/*
C++: CursorEntry(const RefVar&)
MPW-mangled: CursorEntry__FRC6RefVar
*/
    JT _Z11CursorEntryRK6RefVar, 01800858

/*
C++: CursorGotoKey(const RefVar&, const RefVar&)
MPW-mangled: CursorGotoKey__FRC6RefVarT1
*/
    JT CursorGotoKey__FRC6RefVarT1, 01800860

/*
C++: CursorGoto(const RefVar&, const RefVar&)
MPW-mangled: CursorGoto__FRC6RefVarT1
*/
    JT CursorGoto__FRC6RefVarT1, 0180085c

/*
C++: CursorMove(const RefVar&, long)
MPW-mangled: CursorMove__FRC6RefVarl
*/
    JT _Z10CursorMoveRK6RefVarl, 01800864

/*
C++: CursorNext(const RefVar&)
MPW-mangled: CursorNext__FRC6RefVar
*/
    JT _Z10CursorNextRK6RefVar, 01800868

/*
C++: CursorPrev(const RefVar&)
MPW-mangled: CursorPrev__FRC6RefVar
*/
    JT _Z10CursorPrevRK6RefVar, 0180086c

/*
C++: CursorReset(const RefVar&)
MPW-mangled: CursorReset__FRC6RefVar
*/
    JT _Z11CursorResetRK6RefVar, 01800870

/*
C++: DIYGCMark(long)
MPW-mangled: DIYGCMark__Fl
*/
    JT _Z9DIYGCMarkl, 0180088c

/*
C++: DIYGCRegister(void*, void (*)(void*), void (*)(void*))
MPW-mangled: DIYGCRegister__FPvPFPv_vT2
*/
    JT DIYGCRegister__FPvPFPv_vT2, 01800890

/*
C++: DIYGCUnregister(void*)
MPW-mangled: DIYGCUnregister__FPv
*/
    JT _Z15DIYGCUnregisterPv, 01800894

/*
C++: DIYGCUpdate(long)
MPW-mangled: DIYGCUpdate__Fl
*/
    JT _Z11DIYGCUpdatel, 01800898

/*
C++: TCircleBuf::DMABufInfo(unsigned long*, unsigned long*, unsigned char*, unsigned char*)
MPW-mangled: DMABufInfo__10TCircleBufFPUlT1PUcT3
*/
    JT DMABufInfo__10TCircleBufFPUlT1PUcT3, 01802a14

/*
C++: TCircleBuf::DMAGetInfo(unsigned long*)
MPW-mangled: DMAGetInfo__10TCircleBufFPUl
*/
    JT _ZN10TCircleBuf10DMAGetInfoEPm, 01802a18

/*
C++: TCircleBuf::DMAGetUpdate(unsigned long)
MPW-mangled: DMAGetUpdate__10TCircleBufFUl
*/
    JT _ZN10TCircleBuf12DMAGetUpdateEm, 01802a1c

/*
C++: TCircleBuf::DMAPutInfo(unsigned long*, unsigned long*)
MPW-mangled: DMAPutInfo__10TCircleBufFPUlT1
*/
    JT DMAPutInfo__10TCircleBufFPUlT1, 01802a20

/*
C++: TCircleBuf::DMAPutUpdate(unsigned long, unsigned char, unsigned long)
MPW-mangled: DMAPutUpdate__10TCircleBufFUlUcT1
*/
    JT DMAPutUpdate__10TCircleBufFUlUcT1, 01802a24

/*
C++: DSGetMatchedEntries(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: DSGetMatchedEntries__FRC6RefVarN21
*/
    JT DSGetMatchedEntries__FRC6RefVarN21, 01802784

/*
C++: DSPartialStrMatch(const RefVar&, RefVar, RefVar)
MPW-mangled: DSPartialStrMatch__FRC6RefVar6RefVarT2
*/
    JT DSPartialStrMatch__FRC6RefVar6RefVarT2, 01802788

/*
C++: DSResolveString(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: DSResolveString__FRC6RefVarN21
*/
    JT DSResolveString__FRC6RefVarN21, 01802780

/*
C++: DSTagString(const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: DSTagString__FRC6RefVarN31
*/
    JT DSTagString__FRC6RefVarN31, 0180277c

/*
C++: CRingBuffer::DataCount() const
MPW-mangled: DataCount__11CRingBufferCFv
*/
    JT DataCount__11CRingBufferCFv, 01801a90

/*
C++: CShadowRingBuffer::DataCount() const
MPW-mangled: DataCount__17CShadowRingBufferCFv
*/
    JT DataCount__17CShadowRingBufferCFv, 01801a94

/*
C++: TView::DataFrame()
MPW-mangled: DataFrame__5TViewFv
*/
    JT _ZN5TView9DataFrameEv, 01800bb4

/*
C++: TDate::DateElementString(unsigned long, unsigned long, unsigned short*, unsigned long, unsigned char)
MPW-mangled: DateElementString__5TDateFUlT1PUsT1Uc
*/
    JT DateElementString__5TDateFUlT1PUsT1Uc, 01802610

/*
C++: TDate::DaysInMonth() const
MPW-mangled: DaysInMonth__5TDateCFv
*/
    JT DaysInMonth__5TDateCFv, 01802614

/*
C++: TDate::DaysInYear() const
MPW-mangled: DaysInYear__5TDateCFv
*/
    JT DaysInYear__5TDateCFv, 01802660

/*
C++: TClassInfo::DeRegister() const
MPW-mangled: DeRegister__10TClassInfoCFv
*/
    JT _ZNK10TClassInfo10DeRegisterEv, 01800f48

/*
C++: static TRealTimeClock::DeRegister(unsigned long)
MPW-mangled: DeRegister__14TRealTimeClockSFUl
*/
    JT _ZN14TRealTimeClock10DeRegisterEm, 01800f4c

/*
C++: TClassInfoRegistry::DeRegister(const TClassInfo*, unsigned char)
MPW-mangled: DeRegister__18TClassInfoRegistryFPC10TClassInfoUc
*/
    JT _ZN18TClassInfoRegistry10DeRegisterEPK10TClassInfoh, 01800f50

/*
C++: DeallocatePackage(TStore*, unsigned long)
MPW-mangled: DeallocatePackage__FP6TStoreUl
*/
    JT _Z17DeallocatePackageP6TStorem, 01800f34

/*
C++: TCircleBuf::Deallocate()
MPW-mangled: Deallocate__10TCircleBufFv
*/
    JT _ZN10TCircleBuf10DeallocateEv, 01802a28
    JT DebugStr, 01800028
    JT Debugger, 01800024

/*
C++: DecrementCurrentStackPos()
MPW-mangled: DecrementCurrentStackPos__Fv
*/
    JT _Z24DecrementCurrentStackPosv, 01802494

/*
C++: DeepClone(const RefVar&)
MPW-mangled: DeepClone__FRC6RefVar
*/
    JT _Z9DeepCloneRK6RefVar, 01800874

/*
C++: TEndpointClient::DefaultComplete(TEndpointEvent*)
MPW-mangled: DefaultComplete__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient15DefaultCompleteEP14TEndpointEvent, 018002b8

/*
C++: TEndpointClient::Default(TEndpointEvent*)
MPW-mangled: Default__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient7DefaultEP14TEndpointEvent, 018002b4

/*
C++: TScriptEndpointClient::Default(TEndpointEvent*)
MPW-mangled: Default__21TScriptEndpointClientFP14TEndpointEvent
*/
    JT _ZN21TScriptEndpointClient7DefaultEP14TEndpointEvent, 01801f70

/*
C++: TAEventHandler::DeferReply()
MPW-mangled: DeferReply__14TAEventHandlerFv
*/
    JT _ZN14TAEventHandler10DeferReplyEv, 01801a98

/*
C++: DeinstallPackage(unsigned long)
MPW-mangled: DeinstallPackage__FUl
*/
    JT _Z16DeinstallPackagem, 01800f38

/*
C++: CBufferList::DeleteAll()
MPW-mangled: DeleteAll__11CBufferListFv
*/
    JT _ZN11CBufferList9DeleteAllEv, 01801aa0

/*
C++: CArrayIterator::DeleteArray()
MPW-mangled: DeleteArray__14CArrayIteratorFv
*/
    JT _ZN14CArrayIterator11DeleteArrayEv, 01801aa4

/*
C++: CBufferList::DeleteAt(long)
MPW-mangled: DeleteAt__11CBufferListFl
*/
    JT _ZN11CBufferList8DeleteAtEl, 01801aa8

/*
C++: DeleteExternalNewtEventHandler(unsigned long, TExternalNewtEventHandler*, unsigned char)
MPW-mangled: DeleteExternalNewtEventHandler__FUlP25TExternalNewtEventHandlerUc
*/
    JT _Z30DeleteExternalNewtEventHandlermP25TExternalNewtEventHandlerh, 01802dfc

/*
C++: CBufferList::DeleteFirst()
MPW-mangled: DeleteFirst__11CBufferListFv
*/
    JT _ZN11CBufferList11DeleteFirstEv, 01801aac

/*
C++: TDoubleQContainer::DeleteFromQueue(void*)
MPW-mangled: DeleteFromQueue__17TDoubleQContainerFPv
*/
    JT _ZN17TDoubleQContainer15DeleteFromQueueEPv, 01800f40

/*
C++: CBufferList::DeleteLast()
MPW-mangled: DeleteLast__11CBufferListFv
*/
    JT _ZN11CBufferList10DeleteLastEv, 01801ab0

/*
C++: TEndpoint::DeleteLeavingTool()
MPW-mangled: DeleteLeavingTool__9TEndpointFv
*/
    JT _ZN9TEndpoint17DeleteLeavingToolEv, 018002c0

/*
C++: DeleteNSDebugAPI(TNSDebugAPI*)
MPW-mangled: DeleteNSDebugAPI__FP11TNSDebugAPI
*/
    JT _Z16DeleteNSDebugAPIP11TNSDebugAPI, 0180283c

/*
C++: DeletePackage(unsigned long)
MPW-mangled: DeletePackage__FUl
*/
    JT _Z13DeletePackagem, 01800f44

/*
C++: DeletePermObject(TStoreWrapper*, unsigned long)
MPW-mangled: DeletePermObject__FP13TStoreWrapperUl
*/
    JT _Z16DeletePermObjectP13TStoreWrapperm, 0180087c
    JT DeletePersistentVMHeap, 018027b8

/*
C++: DeleteTObjectIterator(TObjectIterator*)
MPW-mangled: DeleteTObjectIterator__FP15TObjectIterator
*/
    JT _Z21DeleteTObjectIteratorP15TObjectIterator, 01802978

/*
C++: TSoupIndex::Delete(SKey*, SKey*)
MPW-mangled: Delete__10TSoupIndexFP4SKeyT1
*/
    JT Delete__10TSoupIndexFP4SKeyT1, 01800878

/*
C++: CBufferList::Delete(CBuffer*)
MPW-mangled: Delete__11CBufferListFP7CBuffer
*/
    JT _ZN11CBufferList6DeleteEP7CBuffer, 01801a9c

/*
C++: TClassInfoRegistry::Delete()
MPW-mangled: Delete__18TClassInfoRegistryFv
*/
    JT _ZN18TClassInfoRegistry6DeleteEv, 01800f3c

/*
C++: TATA::Delete()
MPW-mangled: Delete__4TATAFv
*/
    JT _ZN4TATA6DeleteEv, 01802a70

/*
C++: TEndpoint::Delete()
MPW-mangled: Delete__9TEndpointFv
*/
    JT _ZN9TEndpoint6DeleteEv, 018002bc

/*
C++: DeregisterInterrupt(InterruptObject*)
MPW-mangled: DeregisterInterrupt__FP15InterruptObject
*/
    JT _Z19DeregisterInterruptP15InterruptObject, 01802bc8

/*
C++: TCardSocket::DeregisterSocketInterrupt(TSocketInt)
MPW-mangled: DeregisterSocketInterrupt__11TCardSocketF10TSocketInt
*/
    JT _ZN11TCardSocket25DeregisterSocketInterruptE10TSocketInt, 01800f58

/*
C++: DeregisterTricInterrupt(unsigned long)
MPW-mangled: DeregisterTricInterrupt__FUl
*/
    JT _Z23DeregisterTricInterruptm, 01800f5c

/*
C++: TCardSocket::DeselectPCMCIABus()
MPW-mangled: DeselectPCMCIABus__11TCardSocketFv
*/
    JT _ZN11TCardSocket17DeselectPCMCIABusEv, 01800f60

/*
C++: TEndpoint::DestroyBaseEndpoint()
MPW-mangled: DestroyBaseEndpoint__9TEndpointFv
*/
    JT _ZN9TEndpoint19DestroyBaseEndpointEv, 018002c4

/*
C++: TProtocol::DestroyMonitor()
MPW-mangled: DestroyMonitor__9TProtocolFv
*/
    JT _ZN9TProtocol14DestroyMonitorEv, 01800f68

/*
C++: TUObject::DestroyObject()
MPW-mangled: DestroyObject__8TUObjectFv
*/
    JT _ZN8TUObject13DestroyObjectEv, 01800f6c

/*
C++: TUMonitor::DestroyObject()
MPW-mangled: DestroyObject__9TUMonitorFv
*/
    JT _ZN9TUMonitor13DestroyObjectEv, 01800f70
    JT DestroyVMHeap, 01801774

/*
C++: CMemObject::Destroy()
MPW-mangled: Destroy__10CMemObjectFv
*/
    JT _ZN10CMemObject7DestroyEv, 01801ab4

/*
C++: TClassInfo::Destroy(TProtocol*) const
MPW-mangled: Destroy__10TClassInfoCFP9TProtocol
*/
    JT _ZNK10TClassInfo7DestroyEP9TProtocol, 01800f64

/*
C++: TSoupIndex::Destroy()
MPW-mangled: Destroy__10TSoupIndexFv
*/
    JT _ZN10TSoupIndex7DestroyEv, 01800880

/*
C++: TPackageIterator::DirectorySize()
MPW-mangled: DirectorySize__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator13DirectorySizeEv, 01800f74

/*
C++: TView::Dirty(const TRect*)
MPW-mangled: Dirty__5TViewFPC5TRect
*/
    JT _ZN5TView5DirtyEPK5TRect, 01800bb8
    JT DisableAllInterrupts, 01800f78

/*
C++: TCardSocket::DisableBus()
MPW-mangled: DisableBus__11TCardSocketFv
*/
    JT _ZN11TCardSocket10DisableBusEv, 01800f7c
    JT DisableInterrupt, 0180252c

/*
C++: TCardSocket::DisableSocketAbort()
MPW-mangled: DisableSocketAbort__11TCardSocketFv
*/
    JT _ZN11TCardSocket18DisableSocketAbortEv, 01800f80

/*
C++: TCardSocket::DisableSocketAccess()
MPW-mangled: DisableSocketAccess__11TCardSocketFv
*/
    JT _ZN11TCardSocket19DisableSocketAccessEv, 01800f84

/*
C++: TCardSocket::DisableSocketInterrupt(TSocketInt)
MPW-mangled: DisableSocketInterrupt__11TCardSocketF10TSocketInt
*/
    JT _ZN11TCardSocket22DisableSocketInterruptE10TSocketInt, 01800f88

/*
C++: Disassemble(const RefVar&)
MPW-mangled: Disassemble__FRC6RefVar
*/
    JT _Z11DisassembleRK6RefVar, 01800884

/*
C++: TCommTool::DisconnectComplete(long)
MPW-mangled: DisconnectComplete__9TCommToolFl
*/
    JT _ZN9TCommTool18DisconnectCompleteEl, 01800690

/*
C++: TEndpointClient::Disconnect(TEndpointEvent*)
MPW-mangled: Disconnect__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient10DisconnectEP14TEndpointEvent, 018002c8

/*
C++: TCommTool::Disconnect()
MPW-mangled: Disconnect__9TCommToolFv
*/
    JT _ZN9TCommTool10DisconnectEv, 0180068c

/*
C++: TEndpoint::Disconnect(CBufferSegment*, long, long)
MPW-mangled: Disconnect__9TEndpointFP14CBufferSegmentlT2
*/
    JT Disconnect__9TEndpointFP14CBufferSegmentlT2, 018002cc
    JT DisposHandle, 0180177c
    JT DisposPtr, 01801780

/*
C++: TPackageIterator::DisposeDirectory()
MPW-mangled: DisposeDirectory__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator16DisposeDirectoryEv, 01800f8c

/*
C++: DisposeFgPattern()
MPW-mangled: DisposeFgPattern__Fv
*/
    JT _Z16DisposeFgPatternv, 01800bbc

/*
C++: DisposeRefHandle(RefHandle*)
MPW-mangled: DisposeRefHandle__FP9RefHandle
*/
    JT _Z16DisposeRefHandleP9RefHandle, 01800888

/*
C++: DisposeStackStateBlock(StackState*)
MPW-mangled: DisposeStackStateBlock__FP10StackState
*/
    JT _Z22DisposeStackStateBlockP10StackState, 01802730
    JT DisposeWiredPtr, 01801778

/*
C++: TView::Distance(TPoint, TPoint*)
MPW-mangled: Distance__5TViewF6TPointP6TPoint
*/
    JT _ZN5TView8DistanceE6TPointP6TPoint, 01800bc0

/*
C++: TCardSocket::Do16BitRead(unsigned long)
MPW-mangled: Do16BitRead__11TCardSocketFUl
*/
    JT _ZN11TCardSocket11Do16BitReadEm, 01802de4

/*
C++: TCardSocket::Do16BitWrite(unsigned long, unsigned long)
MPW-mangled: Do16BitWrite__11TCardSocketFUlT1
*/
    JT Do16BitWrite__11TCardSocketFUlT1, 01802de0

/*
C++: TATA::DoATALBACommand(TATALBACommandBlock*)
MPW-mangled: DoATALBACommand__4TATAFP19TATALBACommandBlock
*/
    JT _ZN4TATA15DoATALBACommandEP19TATALBACommandBlock, 01802a74

/*
C++: TATA::DoATARegCommand(TATARegCommandBlock*)
MPW-mangled: DoATARegCommand__4TATAFP19TATARegCommandBlock
*/
    JT _ZN4TATA15DoATARegCommandEP19TATARegCommandBlock, 01802a78

/*
C++: DoBlock(const RefVar&, const RefVar&)
MPW-mangled: DoBlock__FRC6RefVarT1
*/
    JT DoBlock__FRC6RefVarT1, 0180089c

/*
C++: TView::DoCommand(TCommand*)
MPW-mangled: DoCommand__5TViewFP8TCommand
*/
    JT _ZN5TView9DoCommandEP8TCommand, 01800bc4

/*
C++: TCommTool::DoControl(unsigned long, unsigned long)
MPW-mangled: DoControl__9TCommToolFUlT1
*/
    JT DoControl__9TCommToolFUlT1, 01800694

/*
C++: TEndpointEventHandler::DoEventLoop(unsigned long)
MPW-mangled: DoEventLoop__21TEndpointEventHandlerFUl
*/
    JT _ZN21TEndpointEventHandler11DoEventLoopEm, 018002d0

/*
C++: TCommTool::DoKillControl(unsigned long)
MPW-mangled: DoKillControl__9TCommToolFUl
*/
    JT _ZN9TCommTool13DoKillControlEm, 01800698

/*
C++: TCommTool::DoKillGetCommEvent()
MPW-mangled: DoKillGetCommEvent__9TCommToolFv
*/
    JT _ZN9TCommTool18DoKillGetCommEventEv, 0180069c

/*
C++: DoMessageIfDefined(const RefVar&, const RefVar&, const RefVar&, long*)
MPW-mangled: DoMessageIfDefined__FRC6RefVarN21Pl
*/
    JT DoMessageIfDefined__FRC6RefVarN21Pl, 018008a4

/*
C++: DoMessage(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: DoMessage__FRC6RefVarN21
*/
    JT DoMessage__FRC6RefVarN21, 018008a0

/*
C++: TView::DoMoveCommand(TPoint)
MPW-mangled: DoMoveCommand__5TViewF6TPoint
*/
    JT _ZN5TView13DoMoveCommandE6TPoint, 01800bc8

/*
C++: DoProtoMessageIfDefined(const RefVar&, const RefVar&, const RefVar&, long*)
MPW-mangled: DoProtoMessageIfDefined__FRC6RefVarN21Pl
*/
    JT DoProtoMessageIfDefined__FRC6RefVarN21Pl, 018008b0

/*
C++: DoProtoMessage(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: DoProtoMessage__FRC6RefVarN21
*/
    JT DoProtoMessage__FRC6RefVarN21, 018008ac

/*
C++: DoScript(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: DoScript__FRC6RefVarN21
*/
    JT DoScript__FRC6RefVarN21, 018008b4

/*
C++: TCommTool::DoStatus(unsigned long, unsigned long)
MPW-mangled: DoStatus__9TCommToolFUlT1
*/
    JT DoStatus__9TCommToolFUlT1, 018006a0

/*
C++: TPMIterator::Done()
MPW-mangled: Done__11TPMIteratorFv
*/
    JT _ZN11TPMIterator4DoneEv, 01800f90

/*
C++: TPackageLoader::Done(unsigned char*, unsigned char*)
MPW-mangled: Done__14TPackageLoaderFPUcT1
*/
    JT Done__14TPackageLoaderFPUcT1, 01800f94

/*
C++: TObjectIterator::Done()
MPW-mangled: Done__15TObjectIteratorFv
*/
    JT _ZN15TObjectIterator4DoneEv, 018008a8

/*
C++: TView::Drag(TStrokePublic*, const TRect&)
MPW-mangled: Drag__5TViewFP13TStrokePublicRC5TRect
*/
    JT _ZN5TView4DragEP13TStrokePublicRK5TRect, 01800bcc

/*
C++: TView::DrawChildren(TBaseRegion, TView*)
MPW-mangled: DrawChildren__5TViewF11TBaseRegionP5TView
*/
    JT _ZN5TView12DrawChildrenE11TBaseRegionP5TView, 018027bc

/*
C++: TView::DrawChildren(const TRect&, TView*)
MPW-mangled: DrawChildren__5TViewFRC5TRectP5TView
*/
    JT _ZN5TView12DrawChildrenERK5TRectP5TView, 01800bd4

/*
C++: TView::DrawHilitedData()
MPW-mangled: DrawHilitedData__5TViewFv
*/
    JT _ZN5TView15DrawHilitedDataEv, 01800bd8

/*
C++: TView::DrawHiliting()
MPW-mangled: DrawHiliting__5TViewFv
*/
    JT _ZN5TView12DrawHilitingEv, 01800bdc

/*
C++: TView::Draw(TBaseRegion, unsigned char)
MPW-mangled: Draw__5TViewF11TBaseRegionUc
*/
    JT _ZN5TView4DrawE11TBaseRegionh, 01802cb0

/*
C++: TView::Draw(const TRect&, unsigned char)
MPW-mangled: Draw__5TViewFRC5TRectUc
*/
    JT _ZN5TView4DrawERK5TRecth, 01800bd0

/*
C++: EQRef(long, long)
MPW-mangled: EQRef__FlT1
*/
    JT _Z5EQRefll, 018008fc

/*
C++: EQ(const RefVar&, const RefVar&)
MPW-mangled: EQ__FRC6RefVarT1
*/
    JT EQ__FRC6RefVarT1, 0180297c

/*
C++: TEndpoint::EasyClose()
MPW-mangled: EasyClose__9TEndpointFv
*/
    JT _ZN9TEndpoint9EasyCloseEv, 018002d4

/*
C++: TEndpoint::EasyConnect(unsigned long, TOptionArray*, unsigned long)
MPW-mangled: EasyConnect__9TEndpointFUlP12TOptionArrayT1
*/
    JT EasyConnect__9TEndpointFUlP12TOptionArrayT1, 018002d8

/*
C++: TEndpoint::EasyOpen(unsigned long)
MPW-mangled: EasyOpen__9TEndpointFUl
*/
    JT _ZN9TEndpoint8EasyOpenEm, 018002dc

/*
C++: TInterpreter::EnableBreakPoints(unsigned char)
MPW-mangled: EnableBreakPoints__12TInterpreterFUc
*/
    JT _ZN12TInterpreter17EnableBreakPointsEh, 018020c0

/*
C++: TCardSocket::EnableBus()
MPW-mangled: EnableBus__11TCardSocketFv
*/
    JT _ZN11TCardSocket9EnableBusEv, 01800f98
    JT EnableDisableAndClearIRQ, 01801ff0

/*
C++: EnableFramesFunctionProfiling(unsigned char)
MPW-mangled: EnableFramesFunctionProfiling__FUc
*/
    JT _Z29EnableFramesFunctionProfilingh, 018020e4
    JT EnableInterrupt, 018024a4

/*
C++: EnableInterruptBits(unsigned long)
MPW-mangled: EnableInterruptBits__FUl
*/
    JT _Z19EnableInterruptBitsm, 01802bcc

/*
C++: TCardSocket::EnableSocketAbort()
MPW-mangled: EnableSocketAbort__11TCardSocketFv
*/
    JT _ZN11TCardSocket17EnableSocketAbortEv, 01800f9c

/*
C++: TCardSocket::EnableSocketAccess()
MPW-mangled: EnableSocketAccess__11TCardSocketFv
*/
    JT _ZN11TCardSocket18EnableSocketAccessEv, 01800fa0

/*
C++: TCardSocket::EnableSocketInterrupt(TSocketInt)
MPW-mangled: EnableSocketInterrupt__11TCardSocketF10TSocketInt
*/
    JT _ZN11TCardSocket21EnableSocketInterruptE10TSocketInt, 01800fa4

/*
C++: EncodeLine(long, char*, char*, char*)
MPW-mangled: EncodeLine__FlPcN22
*/
    JT EncodeLine__FlPcN22, 01801e54

/*
C++: TWRecognizer::EndInkStrokeGroup(TStrokeUnit**)
MPW-mangled: EndInkStrokeGroup__12TWRecognizerFPP11TStrokeUnit
*/
    JT _ZN12TWRecognizer17EndInkStrokeGroupEPP11TStrokeUnit, 01802cf0

/*
C++: TWRecognizer::EndSubs(TWRecUnit*)
MPW-mangled: EndSubs__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer7EndSubsEP9TWRecUnit, 01802cf4

/*
C++: EnsureInternal(const RefVar&)
MPW-mangled: EnsureInternal__FRC6RefVar
*/
    JT _Z14EnsureInternalRK6RefVar, 018008b8
    JT EnterKMEMode, 018027a4

/*
C++: EntryChangeVerbatim(const RefVar&)
MPW-mangled: EntryChangeVerbatim__FRC6RefVar
*/
    JT _Z19EntryChangeVerbatimRK6RefVar, 018008c0

/*
C++: EntryChangeWithModTime(const RefVar&)
MPW-mangled: EntryChangeWithModTime__FRC6RefVar
*/
    JT _Z22EntryChangeWithModTimeRK6RefVar, 018008c4

/*
C++: EntryChange(const RefVar&)
MPW-mangled: EntryChange__FRC6RefVar
*/
    JT _Z11EntryChangeRK6RefVar, 018008bc

/*
C++: EntryCopy(const RefVar&, const RefVar&)
MPW-mangled: EntryCopy__FRC6RefVarT1
*/
    JT EntryCopy__FRC6RefVarT1, 018008c8

/*
C++: EntryDirty(long)
MPW-mangled: EntryDirty__Fl
*/
    JT _Z10EntryDirtyl, 018008cc

/*
C++: static MemObjManager::EntryLocByIndex(MemObjType, unsigned long)
MPW-mangled: EntryLocByIndex__13MemObjManagerSF10MemObjTypeUl
*/
    JT _ZN13MemObjManager15EntryLocByIndexE10MemObjTypem, 01800fa8

/*
C++: static MemObjManager::EntryLocByName(MemObjType, unsigned long)
MPW-mangled: EntryLocByName__13MemObjManagerSF10MemObjTypeUl
*/
    JT _ZN13MemObjManager14EntryLocByNameE10MemObjTypem, 01800fac

/*
C++: EntryModTime(const RefVar&)
MPW-mangled: EntryModTime__FRC6RefVar
*/
    JT _Z12EntryModTimeRK6RefVar, 018008d0

/*
C++: EntryMove(const RefVar&, const RefVar&)
MPW-mangled: EntryMove__FRC6RefVarT1
*/
    JT EntryMove__FRC6RefVarT1, 018008d4

/*
C++: TClassInfo::EntryProc() const
MPW-mangled: EntryProc__10TClassInfoCFv
*/
    JT EntryProc__10TClassInfoCFv, 01800fb0

/*
C++: EntryRemoveFromSoup(const RefVar&)
MPW-mangled: EntryRemoveFromSoup__FRC6RefVar
*/
    JT _Z19EntryRemoveFromSoupRK6RefVar, 018008d8

/*
C++: EntryReplaceWithModTime(const RefVar&, const RefVar&)
MPW-mangled: EntryReplaceWithModTime__FRC6RefVarT1
*/
    JT EntryReplaceWithModTime__FRC6RefVarT1, 018008e0

/*
C++: EntryReplace(const RefVar&, const RefVar&)
MPW-mangled: EntryReplace__FRC6RefVarT1
*/
    JT EntryReplace__FRC6RefVarT1, 018008dc

/*
C++: EntrySize(const RefVar&)
MPW-mangled: EntrySize__FRC6RefVar
*/
    JT _Z9EntrySizeRK6RefVar, 018008e4

/*
C++: EntrySoup(const RefVar&)
MPW-mangled: EntrySoup__FRC6RefVar
*/
    JT _Z9EntrySoupRK6RefVar, 018008e8

/*
C++: EntryStore(const RefVar&)
MPW-mangled: EntryStore__FRC6RefVar
*/
    JT _Z10EntryStoreRK6RefVar, 018008ec

/*
C++: EntryTextSize(const RefVar&)
MPW-mangled: EntryTextSize__FRC6RefVar
*/
    JT _Z13EntryTextSizeRK6RefVar, 018008f0

/*
C++: EntryUndoChanges(const RefVar&)
MPW-mangled: EntryUndoChanges__FRC6RefVar
*/
    JT _Z16EntryUndoChangesRK6RefVar, 018008f4

/*
C++: EntryUniqueID(const RefVar&)
MPW-mangled: EntryUniqueID__FRC6RefVar
*/
    JT _Z13EntryUniqueIDRK6RefVar, 018008f8

/*
C++: EnvironmentHasDomain(unsigned long, unsigned long, unsigned char*, unsigned char*)
MPW-mangled: EnvironmentHasDomain__FUlT1PUcT3
*/
    JT EnvironmentHasDomain__FUlT1PUcT3, 01800fb4
    JT EqualBytes, 01801630

/*
C++: TCompressor::EstimatedCompressedSize(char**)
MPW-mangled: EstimatedCompressedSize__11TCompressorFPPc
*/
    JT _ZN11TCompressor23EstimatedCompressedSizeEPPc, 01801ab8

/*
C++: ExceptionNotify(Exception*)
MPW-mangled: ExceptionNotify__FP9Exception
*/
    JT _Z15ExceptionNotifyP9Exception, 01802060

/*
C++: Exists(TViewList*, const RefVar&)
MPW-mangled: Exists__FP9TViewListRC6RefVar
*/
    JT _Z6ExistsP9TViewListRK6RefVar, 01800be8
    JT ExitHandler, 01801784
    JT ExitKMEMode, 018027a8

/*
C++: ExpandIRCodeWord(IRCodeWord*, int)
MPW-mangled: ExpandIRCodeWord__FP10IRCodeWordi
*/
    JT _Z16ExpandIRCodeWordP10IRCodeWordi, 01801ef0

/*
C++: TPartHandler::Expand(void*, CPipe*, PartInfo*)
MPW-mangled: Expand__12TPartHandlerFPvP5CPipeP8PartInfo
*/
    JT _ZN12TPartHandler6ExpandEPvP5CPipeP8PartInfo, 01800fb8

/*
C++: TFramePartHandler::Expand(void*, CPipe*, PartInfo*)
MPW-mangled: Expand__17TFramePartHandlerFPvP5CPipeP8PartInfo
*/
    JT _ZN17TFramePartHandler6ExpandEPvP5CPipeP8PartInfo, 01800900
    JT ExtendVMHeap, 01801634
    JT FAdd, 01802024

/*
C++: FAddDeferredAction(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: FAddDeferredAction__FRC6RefVarN21
*/
    JT FAddDeferredAction__FRC6RefVarN21, 01801fb0

/*
C++: FAddDictionary(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: FAddDictionary__FRC6RefVarN21
*/
    JT FAddDictionary__FRC6RefVarN21, 01801fb8
    JT FAref, 01802028
    JT FAutoShutDown, 018020f4

/*
C++: FDisposeDictionary(const RefVar&, const RefVar&)
MPW-mangled: FDisposeDictionary__FRC6RefVarT1
*/
    JT FDisposeDictionary__FRC6RefVarT1, 01801fbc
    JT FDivide, 01802030
    JT FEqual, 01801fd4
    JT FGetGlobals, 01801fa4
    JT FGreaterOrEqual, 01801fe8
    JT FGreaterThan, 01801fe0
    JT FInitSerialDebugging, 01800030
    JT FLessOrEqual, 01801fe4
    JT FLessThan, 01801fdc
    JT FMultiply, 01802034
    JT FNegate, 01802038
    JT FNewIterator, 0180204c
    JT FOpenX, 01801f8c

/*
C++: FPlaySound(const RefVar&, const RefVar&)
MPW-mangled: FPlaySound__FRC6RefVarT1
*/
    JT FPlaySound__FRC6RefVarT1, 01801f98
    JT FRedoChildrenX, 01801f9c

/*
C++: FRefreshViews(const RefVar&)
MPW-mangled: FRefreshViews__FRC6RefVar
*/
    JT _Z13FRefreshViewsRK6RefVar, 01801fac
    JT FSetAref, 0180203c

/*
C++: FSetValue(const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: FSetValue__FRC6RefVarN31
*/
    JT FSetValue__FRC6RefVarN31, 01801fa8

/*
C++: FStrConcat(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: FStrConcat__FRC6RefVarN21
*/
    JT FStrConcat__FRC6RefVarN21, 01801fb4

/*
C++: FStrEqual(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: FStrEqual__FRC6RefVarN21
*/
    JT FStrEqual__FRC6RefVarN21, 01801fc8
    JT FStrFilled, 01801fa0

/*
C++: FStringer(const RefVar&, const RefVar&)
MPW-mangled: FStringer__FRC6RefVarT1
*/
    JT FStringer__FRC6RefVarT1, 01801fd0
    JT FSubtract, 0180202c
    JT FSyncChildrenX, 01801fc4

/*
C++: FTime(const RefVar&)
MPW-mangled: FTime__FRC6RefVar
*/
    JT _Z5FTimeRK6RefVar, 01801fcc
    JT FUnorderedLessOrGreater, 01801fd8

/*
C++: TUDomainManager::FaultMonProc(long, void*)
MPW-mangled: FaultMonProc__15TUDomainManagerFlPv
*/
    JT _ZN15TUDomainManager12FaultMonProcElPv, 01800fbc
    JT FillBytes, 01801638
    JT FillLongs, 0180163c

/*
C++: TView::FindClosestView(TPoint, unsigned long, long*, TPoint*, unsigned char*)
MPW-mangled: FindClosestView__5TViewF6TPointUlPlP6TPointPUc
*/
    JT _ZN5TView15FindClosestViewE6TPointmPlP6TPointPh, 01800bec

/*
C++: static MemObjManager::FindDomainId(unsigned long, unsigned long*)
MPW-mangled: FindDomainId__13MemObjManagerSFUlPUl
*/
    JT _ZN13MemObjManager12FindDomainIdEmPm, 01800fc4

/*
C++: static MemObjManager::FindEntryByName(MemObjType, unsigned long, void*)
MPW-mangled: FindEntryByName__13MemObjManagerSF10MemObjTypeUlPv
*/
    JT _ZN13MemObjManager15FindEntryByNameE10MemObjTypemPv, 01800fc8

/*
C++: static MemObjManager::FindEnvironmentId(unsigned long, unsigned long*)
MPW-mangled: FindEnvironmentId__13MemObjManagerSFUlPUl
*/
    JT _ZN13MemObjManager17FindEnvironmentIdEmPm, 01800fcc

/*
C++: static MemObjManager::FindHeapRef(unsigned long, void**)
MPW-mangled: FindHeapRef__13MemObjManagerSFUlPPv
*/
    JT _ZN13MemObjManager11FindHeapRefEmPPv, 01800fd0

/*
C++: TView::FindID(long)
MPW-mangled: FindID__5TViewFl
*/
    JT _ZN5TView6FindIDEl, 01800bf0

/*
C++: FindImplementor(const RefVar&, const RefVar&)
MPW-mangled: FindImplementor__FRC6RefVarT1
*/
    JT FindImplementor__FRC6RefVarT1, 01801ed0

/*
C++: TUSoundChannel::FindNode(unsigned long)
MPW-mangled: FindNode__14TUSoundChannelFUl
*/
    JT _ZN14TUSoundChannel8FindNodeEm, 01802c30

/*
C++: TOptionIterator::FindOption(unsigned long)
MPW-mangled: FindOption__15TOptionIteratorFUl
*/
    JT _ZN15TOptionIterator10FindOptionEm, 018002e0

/*
C++: FindProtoImplementor(const RefVar&, const RefVar&)
MPW-mangled: FindProtoImplementor__FRC6RefVarT1
*/
    JT FindProtoImplementor__FRC6RefVarT1, 01801ed4

/*
C++: TUSoundChannel::FindRefCon(unsigned long)
MPW-mangled: FindRefCon__14TUSoundChannelFUl
*/
    JT _ZN14TUSoundChannel10FindRefConEm, 01802c34

/*
C++: FindSlotName(const RefVar&, const RefVar&)
MPW-mangled: FindSlotName__FRC6RefVarT1
*/
    JT FindSlotName__FRC6RefVarT1, 01800908

/*
C++: static TRealTimeClock::FindSlot(unsigned long)
MPW-mangled: FindSlot__14TRealTimeClockSFUl
*/
    JT _ZN14TRealTimeClock8FindSlotEm, 01800fd4

/*
C++: TNSDebugAPI::FindVar(long, const RefVar&)
MPW-mangled: FindVar__11TNSDebugAPIFlRC6RefVar
*/
    JT _ZN11TNSDebugAPI7FindVarElRK6RefVar, 01802820

/*
C++: TView::FindView(TPoint, unsigned long, TPoint*)
MPW-mangled: FindView__5TViewF6TPointUlP6TPoint
*/
    JT _ZN5TView8FindViewE6TPointmP6TPoint, 01800bf4

/*
C++: TView::FindView(const RefVar&)
MPW-mangled: FindView__5TViewFRC6RefVar
*/
    JT _ZN5TView8FindViewERK6RefVar, 01800bf8

/*
C++: TClassInfoRegistry::Find(const char*, const char*, int, unsigned long*) const
MPW-mangled: Find__18TClassInfoRegistryCFPCcT1iPUl
*/
    JT Find__18TClassInfoRegistryCFPCcT1iPUl, 01800fc0

/*
C++: RealTimeAlarm::Fire(unsigned long)
MPW-mangled: Fire__13RealTimeAlarmFUl
*/
    JT _ZN13RealTimeAlarm4FireEm, 01800fd8

/*
C++: CArrayIterator::FirstIndex()
MPW-mangled: FirstIndex__14CArrayIteratorFv
*/
    JT _ZN14CArrayIterator10FirstIndexEv, 01801ac0

/*
C++: TOptionIterator::FirstIndex()
MPW-mangled: FirstIndex__15TOptionIteratorFv
*/
    JT _ZN15TOptionIterator10FirstIndexEv, 018002e4

/*
C++: CListIterator::FirstItem()
MPW-mangled: FirstItem__13CListIteratorFv
*/
    JT _ZN13CListIterator9FirstItemEv, 01801ac4

/*
C++: TOptionIterator::FirstOption()
MPW-mangled: FirstOption__15TOptionIteratorFv
*/
    JT _ZN15TOptionIterator11FirstOptionEv, 018002e8

/*
C++: TSoupIndex::First(SKey*, SKey*)
MPW-mangled: First__10TSoupIndexFP4SKeyT1
*/
    JT First__10TSoupIndexFP4SKeyT1, 0180090c

/*
C++: CBufferList::First()
MPW-mangled: First__11CBufferListFv
*/
    JT _ZN11CBufferList5FirstEv, 01801abc

/*
C++: TClassInfoRegistry::First(long, unsigned long*) const
MPW-mangled: First__18TClassInfoRegistryCFlPUl
*/
    JT First__18TClassInfoRegistryCFlPUl, 01800fdc
    JT FixedAtan2, 01801788
    JT FixedDivide, 0180178c
    JT FixedMultiply, 01801790
    JT FixedMultiplyDivide, 01801794

/*
C++: TCircleBuf::FlushBytes()
MPW-mangled: FlushBytes__10TCircleBufFv
*/
    JT _ZN10TCircleBuf10FlushBytesEv, 01802b30
    JT FlushCache, 01800fe4

/*
C++: TCommTool::FlushChannel(CommToolRequestType, long)
MPW-mangled: FlushChannel__9TCommToolF19CommToolRequestTypel
*/
    JT _ZN9TCommTool12FlushChannelE19CommToolRequestTypel, 018006a4
    JT FlushEntireTLB, 01800fe8

/*
C++: TEndpointPipe::FlushRead()
MPW-mangled: FlushRead__13TEndpointPipeFv
*/
    JT _ZN13TEndpointPipe9FlushReadEv, 018002ec

/*
C++: CNullPipe::FlushRead()
MPW-mangled: FlushRead__9CNullPipeFv
*/
    JT _ZN9CNullPipe9FlushReadEv, 01801ac8

/*
C++: CPartPipe::FlushRead()
MPW-mangled: FlushRead__9CPartPipeFv
*/
    JT _ZN9CPartPipe9FlushReadEv, 01800fec

/*
C++: CTaskPipe::FlushRead()
MPW-mangled: FlushRead__9CTaskPipeFv
*/
    JT _ZN9CTaskPipe9FlushReadEv, 01800ff0

/*
C++: TCircleBuf::FlushToNextMarker(unsigned long*)
MPW-mangled: FlushToNextMarker__10TCircleBufFPUl
*/
    JT _ZN10TCircleBuf17FlushToNextMarkerEPm, 01802a2c

/*
C++: TEndpointPipe::FlushWrite()
MPW-mangled: FlushWrite__13TEndpointPipeFv
*/
    JT _ZN13TEndpointPipe10FlushWriteEv, 018002f0

/*
C++: CNullPipe::FlushWrite()
MPW-mangled: FlushWrite__9CNullPipeFv
*/
    JT _ZN9CNullPipe10FlushWriteEv, 01801acc

/*
C++: CPartPipe::FlushWrite()
MPW-mangled: FlushWrite__9CPartPipeFv
*/
    JT _ZN9CPartPipe10FlushWriteEv, 01800ff4

/*
C++: CTaskPipe::FlushWrite()
MPW-mangled: FlushWrite__9CTaskPipeFv
*/
    JT _ZN9CTaskPipe10FlushWriteEv, 01800ff8

/*
C++: TPackageIterator::ForDispatchOnly()
MPW-mangled: ForDispatchOnly__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator15ForDispatchOnlyEv, 01800ffc

/*
C++: ForEachLoopDone(const RefVar&)
MPW-mangled: ForEachLoopDone__FRC6RefVar
*/
    JT _Z15ForEachLoopDoneRK6RefVar, 01802040

/*
C++: ForEachLoopNext(const RefVar&)
MPW-mangled: ForEachLoopNext__FRC6RefVar
*/
    JT _Z15ForEachLoopNextRK6RefVar, 01802044

/*
C++: ForEachLoopReset(const RefVar&, const RefVar&)
MPW-mangled: ForEachLoopReset__FRC6RefVarT1
*/
    JT ForEachLoopReset__FRC6RefVarT1, 01802048

/*
C++: ForgetMapping(unsigned long, unsigned long, unsigned long)
MPW-mangled: ForgetMapping__FUlN21
*/
    JT ForgetMapping__FUlN21, 01801008

/*
C++: TUDomainManager::ForgetPermMap(unsigned long, unsigned long)
MPW-mangled: ForgetPermMap__15TUDomainManagerFUlT1
*/
    JT ForgetPermMap__15TUDomainManagerFUlT1, 0180100c

/*
C++: static TUDomainManager::ForgetPermMap(unsigned long, unsigned long, unsigned long)
MPW-mangled: ForgetPermMap__15TUDomainManagerSFUlN21
*/
    JT ForgetPermMap__15TUDomainManagerSFUlN21, 01801010

/*
C++: ForgetPermMapping(unsigned long, unsigned long, unsigned long)
MPW-mangled: ForgetPermMapping__FUlN21
*/
    JT ForgetPermMapping__FUlN21, 01801014

/*
C++: TUDomainManager::ForgetPhysMapRange(unsigned long, unsigned long, unsigned long)
MPW-mangled: ForgetPhysMapRange__15TUDomainManagerFUlN21
*/
    JT ForgetPhysMapRange__15TUDomainManagerFUlN21, 01802574

/*
C++: static TUDomainManager::ForgetPhysMapRange(unsigned long, unsigned long, unsigned long, unsigned long)
MPW-mangled: ForgetPhysMapRange__15TUDomainManagerSFUlN31
*/
    JT ForgetPhysMapRange__15TUDomainManagerSFUlN31, 01802578

/*
C++: TUDomainManager::ForgetPhysMap(unsigned long, unsigned long)
MPW-mangled: ForgetPhysMap__15TUDomainManagerFUlT1
*/
    JT ForgetPhysMap__15TUDomainManagerFUlT1, 01801018

/*
C++: static TUDomainManager::ForgetPhysMap(unsigned long, unsigned long, unsigned long)
MPW-mangled: ForgetPhysMap__15TUDomainManagerSFUlN21
*/
    JT ForgetPhysMap__15TUDomainManagerSFUlN21, 0180101c

/*
C++: ForgetPhysMapping(unsigned long, unsigned long, unsigned long)
MPW-mangled: ForgetPhysMapping__FUlN21
*/
    JT ForgetPhysMapping__FUlN21, 01801020

/*
C++: TUDomainManager::Forget(unsigned long, unsigned long)
MPW-mangled: Forget__15TUDomainManagerFUlT1
*/
    JT Forget__15TUDomainManagerFUlT1, 01801000

/*
C++: static TUDomainManager::Forget(unsigned long, unsigned long, unsigned long)
MPW-mangled: Forget__15TUDomainManagerSFUlN21
*/
    JT Forget__15TUDomainManagerSFUlN21, 01801004

/*
C++: TATA::Format(unsigned char*, unsigned long, unsigned long, unsigned long, unsigned char)
MPW-mangled: Format__4TATAFPUcUlN22Uc
*/
    JT Format__4TATAFPUcUlN22Uc, 01802a7c
    JT FractDivide, 01801798
    JT FractMultiply, 0180179c
    JT FractSineCosine, 018017a0
    JT FractSquareRoot, 018017a4

/*
C++: FrameHasPath(const RefVar&, const RefVar&)
MPW-mangled: FrameHasPath__FRC6RefVarT1
*/
    JT FrameHasPath__FRC6RefVarT1, 01800910

/*
C++: FrameHasSlotRef(long, long)
MPW-mangled: FrameHasSlotRef__FlT1
*/
    JT FrameHasSlotRef__FlT1, 01800914

/*
C++: FrameHasSlot(const RefVar&, const RefVar&)
MPW-mangled: FrameHasSlot__FRC6RefVarT1
*/
    JT FrameHasSlot__FRC6RefVarT1, 01802980

/*
C++: CRingBuffer::FreeCount() const
MPW-mangled: FreeCount__11CRingBufferCFv
*/
    JT FreeCount__11CRingBufferCFv, 01801ad0

/*
C++: CShadowRingBuffer::FreeCount() const
MPW-mangled: FreeCount__17CShadowRingBufferCFv
*/
    JT FreeCount__17CShadowRingBufferCFv, 01801ad4

/*
C++: FreeInstance(TProtocol*)
MPW-mangled: FreeInstance__FP9TProtocol
*/
    JT _Z12FreeInstanceP9TProtocol, 01801028

/*
C++: TUSoundChannel::FreeNode(SoundNode*)
MPW-mangled: FreeNode__14TUSoundChannelFP9SoundNode
*/
    JT _ZN14TUSoundChannel8FreeNodeEP9SoundNode, 01802c38

/*
C++: static TUPageManager::FreePageCount(unsigned long*)
MPW-mangled: FreePageCount__13TUPageManagerSFPUl
*/
    JT _ZN13TUPageManager13FreePageCountEPm, 0180102c
    JT FreePagedMem, 01801030

/*
C++: TClassInfo::FreeProc() const
MPW-mangled: FreeProc__10TClassInfoCFv
*/
    JT FreeProc__10TClassInfoCFv, 01801034

/*
C++: TView::FrontMostApp()
MPW-mangled: FrontMostApp__5TViewFv
*/
    JT _ZN5TView12FrontMostAppEv, 01800c00

/*
C++: TView::FrontMost()
MPW-mangled: FrontMost__5TViewFv
*/
    JT _ZN5TView9FrontMostEv, 01800bfc

/*
C++: TNSDebugAPI::Function(long)
MPW-mangled: Function__11TNSDebugAPIFl
*/
    JT _ZN11TNSDebugAPI8FunctionEl, 018027f4

/*
C++: GC()
MPW-mangled: GC__Fv
*/
    JT _Z2GCv, 01800918

/*
C++: TUGestalt::Gestalt(unsigned long, void*, unsigned long*)
MPW-mangled: Gestalt__9TUGestaltFUlPvPUl
*/
    JT _ZN9TUGestalt7GestaltEmPvPm, 01802544

/*
C++: TUGestalt::Gestalt(unsigned long, void*, unsigned long)
MPW-mangled: Gestalt__9TUGestaltFUlPvT1
*/
    JT _ZN9TUGestalt7GestaltEmPvm, 01801038

/*
C++: GetADCObject()
MPW-mangled: GetADCObject__Fv
*/
    JT _Z12GetADCObjectv, 01802ce4

/*
C++: TEzEndpointPipe::GetADSPEndpoint()
MPW-mangled: GetADSPEndpoint__15TEzEndpointPipeFv
*/
    JT _ZN15TEzEndpointPipe15GetADSPEndpointEv, 018002f4

/*
C++: GetArraySlotRef(long, long)
MPW-mangled: GetArraySlotRef__FlT1
*/
    JT _Z15GetArraySlotRefll, 01800920

/*
C++: GetArraySlot(const RefVar&, long)
MPW-mangled: GetArraySlot__FRC6RefVarl
*/
    JT _Z12GetArraySlotRK6RefVarl, 01802984

/*
C++: TATA::GetAttributes()
MPW-mangled: GetAttributes__4TATAFv
*/
    JT _ZN4TATA13GetAttributesEv, 01802a80

/*
C++: TPartHandler::GetBackupInfo(const PartId&, unsigned long, long, PartInfo*, unsigned long, unsigned char*)
MPW-mangled: GetBackupInfo__12TPartHandlerFRC6PartIdUllP8PartInfoT2PUc
*/
    JT GetBackupInfo__12TPartHandlerFRC6PartIdUllP8PartInfoT2PUc, 01801044

/*
C++: GetBatteryDriver()
MPW-mangled: GetBatteryDriver__Fv
*/
    JT _Z16GetBatteryDriverv, 018025f0

/*
C++: TCommTool::GetBytesImmediate(CBufferList*, long)
MPW-mangled: GetBytesImmediate__9TCommToolFP11CBufferListl
*/
    JT _ZN9TCommTool17GetBytesImmediateEP11CBufferListl, 018006a8

/*
C++: TCircleBuf::GetBytes(TCircleBuf*)
MPW-mangled: GetBytes__10TCircleBufFP10TCircleBuf
*/
    JT _ZN10TCircleBuf8GetBytesEP10TCircleBuf, 01802a30
    JT GetCPUMode, 018017a8

/*
C++: TCardPackage::GetCPUType() const
MPW-mangled: GetCPUType__12TCardPackageCFv
*/
    JT GetCPUType__12TCardPackageCFv, 0180107c

/*
C++: GetCString(const RefVar&)
MPW-mangled: GetCString__FRC6RefVar
*/
    JT _Z10GetCStringRK6RefVar, 01800850

/*
C++: TView::GetCacheProto(long)
MPW-mangled: GetCacheProto__5TViewFl
*/
    JT _ZN5TView13GetCacheProtoEl, 01800c04

/*
C++: TView::GetCacheVariable(long)
MPW-mangled: GetCacheVariable__5TViewFl
*/
    JT _ZN5TView16GetCacheVariableEl, 01800c08

/*
C++: TClassInfo::GetCapability(const char*) const
MPW-mangled: GetCapability__10TClassInfoCFPCc
*/
    JT GetCapability__10TClassInfoCFPCc, 0180104c

/*
C++: TClassInfo::GetCapability(long) const
MPW-mangled: GetCapability__10TClassInfoCFl
*/
    JT GetCapability__10TClassInfoCFl, 01801048

/*
C++: TCardPCMCIA::GetCardCIS(const unsigned long)
MPW-mangled: GetCardCIS__11TCardPCMCIAFCUl
*/
    JT _ZN11TCardPCMCIA10GetCardCISEKm, 01802dc0

/*
C++: TCardPCMCIA::GetCardConfiguration(const unsigned long)
MPW-mangled: GetCardConfiguration__11TCardPCMCIAFCUl
*/
    JT _ZN11TCardPCMCIA20GetCardConfigurationEKm, 01801050

/*
C++: TCardPCMCIA::GetCardDevice(const unsigned long)
MPW-mangled: GetCardDevice__11TCardPCMCIAFCUl
*/
    JT _ZN11TCardPCMCIA13GetCardDeviceEKm, 01801054

/*
C++: TCardPCMCIA::GetCardFunction(const unsigned long)
MPW-mangled: GetCardFunction__11TCardPCMCIAFCUl
*/
    JT _ZN11TCardPCMCIA15GetCardFunctionEKm, 01802dd0

/*
C++: TCardPCMCIA::GetCardManufacturer() const
MPW-mangled: GetCardManufacturer__11TCardPCMCIACFv
*/
    JT GetCardManufacturer__11TCardPCMCIACFv, 01801058

/*
C++: TCardPCMCIA::GetCardPackage(const unsigned long)
MPW-mangled: GetCardPackage__11TCardPCMCIAFCUl
*/
    JT _ZN11TCardPCMCIA14GetCardPackageEKm, 0180105c

/*
C++: TCardPCMCIA::GetCardProduct() const
MPW-mangled: GetCardProduct__11TCardPCMCIACFv
*/
    JT GetCardProduct__11TCardPCMCIACFv, 01801060

/*
C++: GetCardReinsertionInfo(TCardReinsertionInfo*)
MPW-mangled: GetCardReinsertionInfo__FP20TCardReinsertionInfo
*/
    JT _Z22GetCardReinsertionInfoP20TCardReinsertionInfo, 01802b50

/*
C++: TCardATALoader::GetCardType(TCardPCMCIA*)
MPW-mangled: GetCardType__14TCardATALoaderFP11TCardPCMCIA
*/
    JT _ZN14TCardATALoader11GetCardTypeEP11TCardPCMCIA, 01802a84

/*
C++: TCardPCMCIA::GetCardV1String3() const
MPW-mangled: GetCardV1String3__11TCardPCMCIACFv
*/
    JT GetCardV1String3__11TCardPCMCIACFv, 01801064

/*
C++: TCardPCMCIA::GetCardV1String4() const
MPW-mangled: GetCardV1String4__11TCardPCMCIACFv
*/
    JT GetCardV1String4__11TCardPCMCIACFv, 01801068

/*
C++: TCardPCMCIA::GetCardV2Info() const
MPW-mangled: GetCardV2Info__11TCardPCMCIACFv
*/
    JT GetCardV2Info__11TCardPCMCIACFv, 0180106c

/*
C++: TCardPCMCIA::GetCardV2Vendor() const
MPW-mangled: GetCardV2Vendor__11TCardPCMCIACFv
*/
    JT GetCardV2Vendor__11TCardPCMCIACFv, 01801070

/*
C++: TView::GetChildOrigin(TPoint*)
MPW-mangled: GetChildOrigin__5TViewFP6TPoint
*/
    JT _ZN5TView14GetChildOriginEP6TPoint, 01800c0c

/*
C++: TCommTool::GetCommEvent()
MPW-mangled: GetCommEvent__9TCommToolFv
*/
    JT _ZN9TCommTool12GetCommEventEv, 018006ac

/*
C++: GetCommManagerPort(TUPort*)
MPW-mangled: GetCommManagerPort__FP6TUPort
*/
    JT _Z18GetCommManagerPortP6TUPort, 018002fc

/*
C++: TCardSocket::GetCommonMemSpeed()
MPW-mangled: GetCommonMemSpeed__11TCardSocketFv
*/
    JT _ZN11TCardSocket17GetCommonMemSpeedEv, 01801074

/*
C++: TCommTool::GetComplete(long, unsigned char, unsigned long)
MPW-mangled: GetComplete__9TCommToolFlUcUl
*/
    JT _ZN9TCommTool11GetCompleteElhm, 018006b0

/*
C++: TCommTool::GetConnectState()
MPW-mangled: GetConnectState__9TCommToolFv
*/
    JT _ZN9TCommTool15GetConnectStateEv, 018006b4

/*
C++: GetContiguousMemory(TUPhys*, unsigned long, unsigned long, unsigned char, unsigned char)
MPW-mangled: GetContiguousMemory__FP6TUPhysUlT2UcT4
*/
    JT GetContiguousMemory__FP6TUPhysUlT2UcT4, 01802550

/*
C++: TCardSocket::GetControl()
MPW-mangled: GetControl__11TCardSocketFv
*/
    JT _ZN11TCardSocket10GetControlEv, 01801078
    JT GetCurrentHeap, 018017ac

/*
C++: GetCurrentInterpreterID()
MPW-mangled: GetCurrentInterpreterID__Fv
*/
    JT _Z23GetCurrentInterpreterIDv, 01802ca4

/*
C++: TTransactionResponseBuffer::GetDataPtr(unsigned char)
MPW-mangled: GetDataPtr__26TTransactionResponseBufferFUc
*/
    JT _ZN26TTransactionResponseBuffer10GetDataPtrEh, 018020a4

/*
C++: TStoreHashTableIterator::GetData(char*, long*)
MPW-mangled: GetData__23TStoreHashTableIteratorFPcPl
*/
    JT _ZN23TStoreHashTableIterator7GetDataEPcPl, 01800924
    JT GetDomainAndFaultMonitorFromDomainNumber, 01801080

/*
C++: static MemObjManager::GetDomainInfoByName(unsigned long, DomainInfo*)
MPW-mangled: GetDomainInfoByName__13MemObjManagerSFUlP10DomainInfo
*/
    JT _ZN13MemObjManager19GetDomainInfoByNameEmP10DomainInfo, 01801088

/*
C++: static MemObjManager::GetDomainInfo(unsigned long, DomainInfo*, long*)
MPW-mangled: GetDomainInfo__13MemObjManagerSFUlP10DomainInfoPl
*/
    JT _ZN13MemObjManager13GetDomainInfoEmP10DomainInfoPl, 01801084

/*
C++: GetEffectiveInterruptPriority(InterruptObject*)
MPW-mangled: GetEffectiveInterruptPriority__FP15InterruptObject
*/
    JT _Z29GetEffectiveInterruptPriorityP15InterruptObject, 01802bd0

/*
C++: CDynamicArray::GetElementsAt(long, void*, long)
MPW-mangled: GetElementsAt__13CDynamicArrayFlPvT1
*/
    JT GetElementsAt__13CDynamicArrayFlPvT1, 01801af0

/*
C++: TWRecognizer::GetEndTime(TStrokeUnit*)
MPW-mangled: GetEndTime__12TWRecognizerFP11TStrokeUnit
*/
    JT _ZN12TWRecognizer10GetEndTimeEP11TStrokeUnit, 01802cf8

/*
C++: TWRecognizer::GetEndTime(TUnit*)
MPW-mangled: GetEndTime__12TWRecognizerFP5TUnit
*/
    JT _ZN12TWRecognizer10GetEndTimeEP5TUnit, 01802cfc

/*
C++: TWRecognizer::GetEndTime(TSIUnit*)
MPW-mangled: GetEndTime__12TWRecognizerFP7TSIUnit
*/
    JT _ZN12TWRecognizer10GetEndTimeEP7TSIUnit, 01802d00

/*
C++: TWRecognizer::GetEndTime(TStroke*)
MPW-mangled: GetEndTime__12TWRecognizerFP7TStroke
*/
    JT _ZN12TWRecognizer10GetEndTimeEP7TStroke, 01802d04

/*
C++: TWRecognizer::GetEndTime(TWRecUnit*)
MPW-mangled: GetEndTime__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer10GetEndTimeEP9TWRecUnit, 01802d08

/*
C++: static MemObjManager::GetEnvDomainName(unsigned long, unsigned long, unsigned long*, unsigned char*, long*)
MPW-mangled: GetEnvDomainName__13MemObjManagerSFUlT1PUlPUcPl
*/
    JT GetEnvDomainName__13MemObjManagerSFUlT1PUlPUcPl, 0180108c

/*
C++: static MemObjManager::GetEnvironmentInfo(unsigned long, EnvironmentInfo*, long*)
MPW-mangled: GetEnvironmentInfo__13MemObjManagerSFUlP15EnvironmentInfoPl
*/
    JT _ZN13MemObjManager18GetEnvironmentInfoEmP15EnvironmentInfoPl, 01801094

/*
C++: GetEnvironment(unsigned long*)
MPW-mangled: GetEnvironment__FPUl
*/
    JT _Z14GetEnvironmentPm, 01801090

/*
C++: TAppWorldState::GetError()
MPW-mangled: GetError__14TAppWorldStateFv
*/
    JT _ZN14TAppWorldState8GetErrorEv, 01801af4

/*
C++: TAppWorld::GetError()
MPW-mangled: GetError__9TAppWorldFv
*/
    JT _ZN9TAppWorld8GetErrorEv, 01801af8

/*
C++: GetExceptionHandler()
MPW-mangled: GetExceptionHandler__Fv
*/
    JT _Z19GetExceptionHandlerv, 018017b0

/*
C++: static TUPageManager::GetExternal(unsigned long&, unsigned long, int)
MPW-mangled: GetExternal__13TUPageManagerSFRUlUli
*/
    JT _ZN13TUPageManager11GetExternalERmmi, 01801098

/*
C++: GetFaultState(TProcessorState*)
MPW-mangled: GetFaultState__FP15TProcessorState
*/
    JT _Z13GetFaultStateP15TProcessorState, 0180109c

/*
C++: GetFramePath(const RefVar&, const RefVar&)
MPW-mangled: GetFramePath__FRC6RefVarT1
*/
    JT GetFramePath__FRC6RefVarT1, 01800928

/*
C++: GetFrameSlotRef(long, long)
MPW-mangled: GetFrameSlotRef__FlT1
*/
    JT _Z15GetFrameSlotRefll, 0180092c

/*
C++: GetFrameSlot(const RefVar&, const RefVar&)
MPW-mangled: GetFrameSlot__FRC6RefVarT1
*/
    JT _Z12GetFrameSlotRK6RefVarS1_, 01802988

/*
C++: GetFramesErrorString(long)
MPW-mangled: GetFramesErrorString__Fl
*/
    JT _Z20GetFramesErrorStringl, 01802c90

/*
C++: TView::GetFrontMask() const
MPW-mangled: GetFrontMask__5TViewCFv
*/
    JT GetFrontMask__5TViewCFv, 01800c10

/*
C++: TCardFunction::GetFuncExt(const unsigned long)
MPW-mangled: GetFuncExt__13TCardFunctionFCUl
*/
    JT _ZN13TCardFunction10GetFuncExtEKm, 01802db4

/*
C++: GetFunctionArgCount(const RefVar&)
MPW-mangled: GetFunctionArgCount__FRC6RefVar
*/
    JT _Z19GetFunctionArgCountRK6RefVar, 018027e4

/*
C++: GetGFunctionFrame()
MPW-mangled: GetGFunctionFrame__Fv
*/
    JT _Z17GetGFunctionFramev, 01801eec

/*
C++: GetGInterpreter()
MPW-mangled: GetGInterpreter__Fv
*/
    JT _Z15GetGInterpreterv, 01801ee8

/*
C++: GetGPIOInterfaceObject()
MPW-mangled: GetGPIOInterfaceObject__Fv
*/
    JT _Z22GetGPIOInterfaceObjectv, 01802cd8
    JT GetGlobalTime, 018010a4
    JT GetGlobals, 018010a0
    JT GetGlobalsThatLiveAcrossReboot, 018020f0

/*
C++: GetGrafInfo(long, void*)
MPW-mangled: GetGrafInfo__FlPv
*/
    JT _Z11GetGrafInfolPv, 01801f64
    JT GetHandleName, 01801640
    JT GetHandleOwner, 01801644
    JT GetHandleSize, 018017b4
    JT GetHandleType, 01801648

/*
C++: TDelayTimer::GetHardwareTime()
MPW-mangled: GetHardwareTime__11TDelayTimerFv
*/
    JT _ZN11TDelayTimer15GetHardwareTimeEv, 018025a4
    JT GetHeap, 018017b8
    JT GetHeapAreaInfo, 018010a8
    JT GetHeapEnd, 0180164c
    JT GetHeapExtent, 01801650
    JT GetHeapRefcon, 018017bc
    JT GetHeapReleaseable, 01801654

/*
C++: GetHeapSemaphore(void*)
MPW-mangled: GetHeapSemaphore__FPv
*/
    JT _Z16GetHeapSemaphorePv, 018017c0
    JT GetHeapStart, 01801658

/*
C++: CMemObject::GetId()
MPW-mangled: GetId__10CMemObjectFv
*/
    JT _ZN10CMemObject5GetIdEv, 01801afc

/*
C++: TMemoryObject::GetId()
MPW-mangled: GetId__13TMemoryObjectFv
*/
    JT _ZN13TMemoryObject5GetIdEv, 01802094

/*
C++: CList::GetIdentityIndex(void*)
MPW-mangled: GetIdentityIndex__5CListFPv
*/
    JT _ZN5CList16GetIdentityIndexEPv, 01801b00

/*
C++: TInterpreter::GetImplementor()
MPW-mangled: GetImplementor__12TInterpreterFv
*/
    JT _ZN12TInterpreter14GetImplementorEv, 01802738

/*
C++: CBufferList::GetIndex(CBuffer*)
MPW-mangled: GetIndex__11CBufferListFP7CBuffer
*/
    JT _ZN11CBufferList8GetIndexEP7CBuffer, 01801b04
    JT GetIndirectBlockSize, 0180165c

/*
C++: TEndpoint::GetInfo(TCMOTransportInfo*)
MPW-mangled: GetInfo__9TEndpointFP17TCMOTransportInfo
*/
    JT _ZN9TEndpoint7GetInfoEP17TCMOTransportInfo, 01800300

/*
C++: TClassInfoRegistry::GetInstanceCount(const TClassInfo*)
MPW-mangled: GetInstanceCount__18TClassInfoRegistryFPC10TClassInfo
*/
    JT _ZN18TClassInfoRegistry16GetInstanceCountEPK10TClassInfo, 01802a88

/*
C++: GetInterpreterIDs()
MPW-mangled: GetInterpreterIDs__Fv
*/
    JT _Z17GetInterpreterIDsv, 01802ca8

/*
C++: GetInterruptPriority(InterruptObject*)
MPW-mangled: GetInterruptPriority__FP15InterruptObject
*/
    JT _Z20GetInterruptPriorityP15InterruptObject, 01802bd4

/*
C++: TWRecognizer::GetLabel(TWRecUnit*, unsigned long)
MPW-mangled: GetLabel__12TWRecognizerFP9TWRecUnitUl
*/
    JT _ZN12TWRecognizer8GetLabelEP9TWRecUnitm, 01802d0c
    JT GetLastRExConfigEntry, 0180275c

/*
C++: TInterpreter::GetLocalFromStack(const RefVar&, const RefVar&)
MPW-mangled: GetLocalFromStack__12TInterpreterFRC6RefVarT1
*/
    JT GetLocalFromStack__12TInterpreterFRC6RefVarT1, 01802014

/*
C++: TEzEndpointPipe::GetMNPSerialEndpoint()
MPW-mangled: GetMNPSerialEndpoint__15TEzEndpointPipeFv
*/
    JT _ZN15TEzEndpointPipe20GetMNPSerialEndpointEv, 01800304

/*
C++: TProtocol::GetMonitorId() const
MPW-mangled: GetMonitorId__9TProtocolCFv
*/
    JT GetMonitorId__9TProtocolCFv, 018010ac

/*
C++: TAppWorld::GetMyPort()
MPW-mangled: GetMyPort__9TAppWorldFv
*/
    JT _ZN9TAppWorld9GetMyPortEv, 01801b08

/*
C++: TCardPackage::GetName() const
MPW-mangled: GetName__12TCardPackageCFv
*/
    JT GetName__12TCardPackageCFv, 018010b0

/*
C++: TLookupNames::GetName(TNBPName&, unsigned long)
MPW-mangled: GetName__12TLookupNamesFR8TNBPNameUl
*/
    JT _ZN12TLookupNames7GetNameER8TNBPNamem, 01802070

/*
C++: GetNetworkPersistentInfo(unsigned long*)
MPW-mangled: GetNetworkPersistentInfo__FPUl
*/
    JT _Z24GetNetworkPersistentInfoPm, 018010b4

/*
C++: GetNewPageFromPageMgr(void**, unsigned long*, TPhys**)
MPW-mangled: GetNewPageFromPageMgr__FPPvPUlPP5TPhys
*/
    JT _Z21GetNewPageFromPageMgrPPvPmPP5TPhys, 018010b8

/*
C++: GetNewtRootView()
MPW-mangled: GetNewtRootView__Fv
*/
    JT _Z15GetNewtRootViewv, 01802e04

/*
C++: GetNewtTaskPort()
MPW-mangled: GetNewtTaskPort__Fv
*/
    JT _Z15GetNewtTaskPortv, 01802e00

/*
C++: TCircleBuf::GetNextByte(unsigned char*)
MPW-mangled: GetNextByte__10TCircleBufFPUc
*/
    JT _ZN10TCircleBuf11GetNextByteEPh, 01802a34

/*
C++: TCircleBuf::GetNextByte(unsigned char*, unsigned long*)
MPW-mangled: GetNextByte__10TCircleBufFPUcPUl
*/
    JT _ZN10TCircleBuf11GetNextByteEPhPm, 01802a38

/*
C++: GetNextKernelVAddr()
MPW-mangled: GetNextKernelVAddr__Fv
*/
    JT _Z18GetNextKernelVAddrv, 018010c0

/*
C++: TCommTool::GetNextTermProc(unsigned long, unsigned long&, unsigned char (*&)(void*))
MPW-mangled: GetNextTermProc__9TCommToolFUlRUlRPFPv_Uc
*/
    JT GetNextTermProc__9TCommToolFUlRUlRPFPv_Uc, 018006b8

/*
C++: TLicenseeVAddress::GetNextVirtualAddress(unsigned long)
MPW-mangled: GetNextVirtualAddress__17TLicenseeVAddressFUl
*/
    JT _ZN17TLicenseeVAddress21GetNextVirtualAddressEm, 01802568

/*
C++: TDoubleQContainer::GetNext(void*)
MPW-mangled: GetNext__17TDoubleQContainerFPv
*/
    JT _ZN17TDoubleQContainer7GetNextEPv, 018010bc

/*
C++: TCardPCMCIA::GetNumOfCISs()
MPW-mangled: GetNumOfCISs__11TCardPCMCIAFv
*/
    JT _ZN11TCardPCMCIA12GetNumOfCISsEv, 01802dbc

/*
C++: TCardPCMCIA::GetNumOfCardFunctions()
MPW-mangled: GetNumOfCardFunctions__11TCardPCMCIAFv
*/
    JT _ZN11TCardPCMCIA21GetNumOfCardFunctionsEv, 01802dcc

/*
C++: TCardFunction::GetNumOfFuncExts()
MPW-mangled: GetNumOfFuncExts__13TCardFunctionFv
*/
    JT _ZN13TCardFunction16GetNumOfFuncExtsEv, 01802db0

/*
C++: GetOSPortFromName(unsigned long, TUPort*)
MPW-mangled: GetOSPortFromName__FUlP6TUPort
*/
    JT _Z17GetOSPortFromNamemP6TUPort, 01800308

/*
C++: TCardPackage::GetOSType() const
MPW-mangled: GetOSType__12TCardPackageCFv
*/
    JT GetOSType__12TCardPackageCFv, 018010c8

/*
C++: TCardSocket::GetPCPins()
MPW-mangled: GetPCPins__11TCardSocketFv
*/
    JT _ZN11TCardSocket9GetPCPinsEv, 018010dc

/*
C++: TPackageIterator::GetPackageId()
MPW-mangled: GetPackageId__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator12GetPackageIdEv, 018010cc
    JT GetPackageList, 01802764

/*
C++: TCardCISIterator::GetPackage(TCardPackage*, unsigned char)
MPW-mangled: GetPackage__16TCardCISIteratorFP12TCardPackageUc
*/
    JT _ZN16TCardCISIterator10GetPackageEP12TCardPackageh, 01802508

/*
C++: TPackageIterator::GetPartDataOffset(unsigned long)
MPW-mangled: GetPartDataOffset__16TPackageIteratorFUl
*/
    JT _ZN16TPackageIterator17GetPartDataOffsetEm, 018010d0

/*
C++: TPackageIterator::GetPartInfo(unsigned long, PartInfo* const)
MPW-mangled: GetPartInfo__16TPackageIteratorFUlCP8PartInfo
*/
    JT _ZN16TPackageIterator11GetPartInfoEmKP8PartInfo, 018010d4

/*
C++: TWRecognizer::GetPartialGroup(unsigned char*)
MPW-mangled: GetPartialGroup__12TWRecognizerFPUc
*/
    JT _ZN12TWRecognizer15GetPartialGroupEPh, 01802d10

/*
C++: GetPatchInfo(unsigned long*, unsigned long*)
MPW-mangled: GetPatchInfo__FPUlT1
*/
    JT GetPatchInfo__FPUlT1, 018010d8

/*
C++: static MemObjManager::GetPersistentRef(unsigned long, PersistentDBEntry**, long*)
MPW-mangled: GetPersistentRef__13MemObjManagerSFUlPP17PersistentDBEntryPl
*/
    JT _ZN13MemObjManager16GetPersistentRefEmPP17PersistentDBEntryPl, 018010e0

/*
C++: GetPhys(unsigned long, TPhys*&)
MPW-mangled: GetPhys__FUlRP5TPhys
*/
    JT _Z7GetPhysmRP5TPhys, 018010e4

/*
C++: GetPixelMapBits(PixelMap*)
MPW-mangled: GetPixelMapBits__FP8PixelMap
*/
    JT _Z15GetPixelMapBitsP8PixelMap, 01802cd4

/*
C++: GetPkgInfoFromVAddr(unsigned long)
MPW-mangled: GetPkgInfoFromVAddr__FUl
*/
    JT _Z19GetPkgInfoFromVAddrm, 01802cc8

/*
C++: GetPlatformDriver()
MPW-mangled: GetPlatformDriver__Fv
*/
    JT _Z17GetPlatformDriverv, 01802d90

/*
C++: TAppWorldState::GetPort()
MPW-mangled: GetPort__14TAppWorldStateFv
*/
    JT _ZN14TAppWorldState7GetPortEv, 01801b20

/*
C++: GetPort(GrafPort**)
MPW-mangled: GetPort__FPP8GrafPort
*/
    JT _Z7GetPortPP8GrafPort, 01801e70

/*
C++: TEndpointEventHandler::GetPrivatePortId()
MPW-mangled: GetPrivatePortId__21TEndpointEventHandlerFv
*/
    JT _ZN21TEndpointEventHandler16GetPrivatePortIdEv, 0180030c

/*
C++: TEndpointClient::GetProtAddr(TEndpointEvent*)
MPW-mangled: GetProtAddr__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient11GetProtAddrEP14TEndpointEvent, 01800310

/*
C++: TCommTool::GetProtAddr()
MPW-mangled: GetProtAddr__9TCommToolFv
*/
    JT _ZN9TCommTool11GetProtAddrEv, 018006bc

/*
C++: TEndpoint::GetProtAddr(TOptionArray*, TOptionArray*, unsigned long)
MPW-mangled: GetProtAddr__9TEndpointFP12TOptionArrayT1Ul
*/
    JT GetProtAddr__9TEndpointFP12TOptionArrayT1Ul, 01800314

/*
C++: GetProtoVariable(const RefVar&, const RefVar&, long*)
MPW-mangled: GetProtoVariable__FRC6RefVarT1Pl
*/
    JT GetProtoVariable__FRC6RefVarT1Pl, 01800934

/*
C++: TView::GetProto(const RefVar&) const
MPW-mangled: GetProto__5TViewCFRC6RefVar
*/
    JT GetProto__5TViewCFRC6RefVar, 01800c14

/*
C++: GetProtocolRegistry()
MPW-mangled: GetProtocolRegistry__Fv
*/
    JT _Z19GetProtocolRegistryv, 018025ec
    JT GetPtrName, 01801660
    JT GetPtrOwner, 01801664
    JT GetPtrSize, 018017c4
    JT GetPtrType, 01801668

/*
C++: TMemoryObject::GetPtr()
MPW-mangled: GetPtr__13TMemoryObjectFv
*/
    JT _ZN13TMemoryObject6GetPtrEv, 018020a0
    JT GetRAMSize, 018010e8
    JT GetRExConfigEntry, 01802758
    JT GetRExPtr, 01802760

/*
C++: GetROMDomainManagerId()
MPW-mangled: GetROMDomainManagerId__Fv
*/
    JT _Z21GetROMDomainManagerIdv, 018010fc

/*
C++: GetRealTimeClock()
MPW-mangled: GetRealTimeClock__Fv
*/
    JT _Z16GetRealTimeClockv, 018010ec

/*
C++: TInterpreter::GetReceiver()
MPW-mangled: GetReceiver__12TInterpreterFv
*/
    JT _ZN12TInterpreter11GetReceiverEv, 01802734

/*
C++: TUSemaphoreGroup::GetRefCon(void**)
MPW-mangled: GetRefCon__16TUSemaphoreGroupFPPv
*/
    JT _ZN16TUSemaphoreGroup9GetRefConEPPv, 018010f0

/*
C++: TUTask::GetRegister(unsigned long, unsigned long*)
MPW-mangled: GetRegister__6TUTaskFUlPUl
*/
    JT _ZN6TUTask11GetRegisterEmPm, 018010f4
    JT GetRelocHeap, 0180166c

/*
C++: TUAsyncMessage::GetResult(unsigned long*, unsigned long*, unsigned long*, unsigned long*)
MPW-mangled: GetResult__14TUAsyncMessageFPUlN31
*/
    JT GetResult__14TUAsyncMessageFPUlN31, 018010f8

/*
C++: GetSCPLoaderPort(TUPort*)
MPW-mangled: GetSCPLoaderPort__FP6TUPort
*/
    JT _Z16GetSCPLoaderPortP6TUPort, 01800318

/*
C++: TWRecognizer::GetSamplePtAddress(TStrokeUnit*, unsigned long)
MPW-mangled: GetSamplePtAddress__12TWRecognizerFP11TStrokeUnitUl
*/
    JT _ZN12TWRecognizer18GetSamplePtAddressEP11TStrokeUnitm, 01802d14

/*
C++: TWRecognizer::GetSamplePtAddress(TStroke*, unsigned long)
MPW-mangled: GetSamplePtAddress__12TWRecognizerFP7TStrokeUl
*/
    JT _ZN12TWRecognizer18GetSamplePtAddressEP7TStrokem, 01802d18

/*
C++: TADC::GetSample(ADCMuxType, unsigned long, void (*)(void*, long, unsigned long), void*)
MPW-mangled: GetSample__4TADCF10ADCMuxTypeUlPFPvlUl_vPv
*/
    JT GetSample__4TADCF10ADCMuxTypeUlPFPvlUl_vPv, 01802ce0

/*
C++: TWRecognizer::GetScore(TWRecUnit*, unsigned long)
MPW-mangled: GetScore__12TWRecognizerFP9TWRecUnitUl
*/
    JT _ZN12TWRecognizer8GetScoreEP9TWRecUnitm, 01802d1c

/*
C++: TInterpreter::GetSelfFromStack(const RefVar&)
MPW-mangled: GetSelfFromStack__12TInterpreterFRC6RefVar
*/
    JT _ZN12TInterpreter16GetSelfFromStackERK6RefVar, 01802018

/*
C++: TUSharedMemMsg::GetSenderTaskId(unsigned long*)
MPW-mangled: GetSenderTaskId__14TUSharedMemMsgFPUl
*/
    JT _ZN14TUSharedMemMsg15GetSenderTaskIdEPm, 01801100

/*
C++: GetSerialChipRegistry()
MPW-mangled: GetSerialChipRegistry__Fv
*/
    JT _Z21GetSerialChipRegistryv, 018027dc

/*
C++: TEzEndpointPipe::GetSerialEndpoint()
MPW-mangled: GetSerialEndpoint__15TEzEndpointPipeFv
*/
    JT _ZN15TEzEndpointPipe17GetSerialEndpointEv, 0180031c

/*
C++: TEndpointEventHandler::GetServicePortId()
MPW-mangled: GetServicePortId__21TEndpointEventHandlerFv
*/
    JT _ZN21TEndpointEventHandler16GetServicePortIdEv, 01800320

/*
C++: TAppWorld::GetSizeOf()
MPW-mangled: GetSizeOf__9TAppWorldFv
*/
    JT _ZN9TAppWorld9GetSizeOfEv, 01801b38

/*
C++: CBufferList::GetSize() const
MPW-mangled: GetSize__11CBufferListCFv
*/
    JT _ZNK11CBufferList7GetSizeEv, 01801b24

/*
C++: CRingBuffer::GetSize() const
MPW-mangled: GetSize__11CRingBufferCFv
*/
    JT GetSize__11CRingBufferCFv, 01801b28

/*
C++: TUSharedMem::GetSize(unsigned long*, void**)
MPW-mangled: GetSize__11TUSharedMemFPUlPPv
*/
    JT _ZN11TUSharedMem7GetSizeEPmPPv, 01801104

/*
C++: CBufferSegment::GetSize() const
MPW-mangled: GetSize__14CBufferSegmentCFv
*/
    JT _ZNK14CBufferSegment7GetSizeEv, 01801b2c

/*
C++: TUSharedMemMsg::GetSize(unsigned long*, void**, unsigned long*)
MPW-mangled: GetSize__14TUSharedMemMsgFPUlPPvT1
*/
    JT GetSize__14TUSharedMemMsgFPUlPPvT1, 01801108

/*
C++: CShadowRingBuffer::GetSize() const
MPW-mangled: GetSize__17CShadowRingBufferCFv
*/
    JT GetSize__17CShadowRingBufferCFv, 01801b30

/*
C++: CShadowBufferSegment::GetSize() const
MPW-mangled: GetSize__20CShadowBufferSegmentCFv
*/
    JT GetSize__20CShadowBufferSegmentCFv, 01801b34
    JT GetSkiaHeapRefcon, 01801670
    JT GetSkiaHeapSemaphore, 01801674

/*
C++: GetSocketInfo(unsigned long, TCardSocket**, TCardPCMCIA**)
MPW-mangled: GetSocketInfo__FUlPP11TCardSocketPP11TCardPCMCIA
*/
    JT _Z13GetSocketInfomPP11TCardSocketPP11TCardPCMCIA, 01802524

/*
C++: TPartHandler::GetSourcePtr()
MPW-mangled: GetSourcePtr__12TPartHandlerFv
*/
    JT _ZN12TPartHandler12GetSourcePtrEv, 0180110c

/*
C++: GetStackStateBlock()
MPW-mangled: GetStackStateBlock__Fv
*/
    JT _Z18GetStackStateBlockv, 01802728

/*
C++: TWRecognizer::GetStartTime(TStrokeUnit*)
MPW-mangled: GetStartTime__12TWRecognizerFP11TStrokeUnit
*/
    JT _ZN12TWRecognizer12GetStartTimeEP11TStrokeUnit, 01802d20

/*
C++: TWRecognizer::GetStartTime(TUnit*)
MPW-mangled: GetStartTime__12TWRecognizerFP5TUnit
*/
    JT _ZN12TWRecognizer12GetStartTimeEP5TUnit, 01802d24

/*
C++: TWRecognizer::GetStartTime(TSIUnit*)
MPW-mangled: GetStartTime__12TWRecognizerFP7TSIUnit
*/
    JT _ZN12TWRecognizer12GetStartTimeEP7TSIUnit, 01802d28

/*
C++: TWRecognizer::GetStartTime(TStroke*)
MPW-mangled: GetStartTime__12TWRecognizerFP7TStroke
*/
    JT _ZN12TWRecognizer12GetStartTimeEP7TStroke, 01802d2c

/*
C++: TWRecognizer::GetStartTime(TWRecUnit*)
MPW-mangled: GetStartTime__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer12GetStartTimeEP9TWRecUnit, 01802d30

/*
C++: TCardCISIterator::GetStatus()
MPW-mangled: GetStatus__16TCardCISIteratorFv
*/
    JT _ZN16TCardCISIterator9GetStatusEv, 01802d9c

/*
C++: GetStores()
MPW-mangled: GetStores__Fv
*/
    JT _Z9GetStoresv, 01800938

/*
C++: TWRecognizer::GetSub(TWRecUnit*, unsigned long)
MPW-mangled: GetSub__12TWRecognizerFP9TWRecUnitUl
*/
    JT _ZN12TWRecognizer6GetSubEP9TWRecUnitm, 01802d34

/*
C++: TEndpointEventHandler::GetSyncPortId()
MPW-mangled: GetSyncPortId__21TEndpointEventHandlerFv
*/
    JT _ZN21TEndpointEventHandler13GetSyncPortIdEv, 01800324
    JT GetSystemReleasable, 01801114

/*
C++: GetTInterpreter(long)
MPW-mangled: GetTInterpreter__Fl
*/
    JT _Z15GetTInterpreterl, 01802cac
    JT GetTaskTime, 01801118

/*
C++: TView::GetTextStyleRecord(StyleRecord*)
MPW-mangled: GetTextStyleRecord__5TViewFP11StyleRecord
*/
    JT _ZN5TView18GetTextStyleRecordEP11StyleRecord, 01800c1c

/*
C++: TView::GetTextStyle()
MPW-mangled: GetTextStyle__5TViewFv
*/
    JT _ZN5TView12GetTextStyleEv, 01800c18

/*
C++: TEndpointPipe::GetTimeout()
MPW-mangled: GetTimeout__13TEndpointPipeFv
*/
    JT _ZN13TEndpointPipe10GetTimeoutEv, 01800328

/*
C++: TCommTool::GetToolPort(unsigned long, TUPort&)
MPW-mangled: GetToolPort__9TCommToolFUlR6TUPort
*/
    JT _ZN9TCommTool11GetToolPortEmR6TUPort, 018006c0

/*
C++: TCardCISIterator::GetTupleData(unsigned char*, unsigned long)
MPW-mangled: GetTupleData__16TCardCISIteratorFPUcUl
*/
    JT _ZN16TCardCISIterator12GetTupleDataEPhm, 01802504

/*
C++: TCardCISIterator::GetTuple(unsigned char)
MPW-mangled: GetTuple__16TCardCISIteratorFUc
*/
    JT _ZN16TCardCISIterator8GetTupleEh, 018024fc

/*
C++: GetUnionSoup(const RefVar&)
MPW-mangled: GetUnionSoup__FRC6RefVar
*/
    JT _Z12GetUnionSoupRK6RefVar, 0180093c

/*
C++: TUMsgToken::GetUserRefCon(unsigned long*)
MPW-mangled: GetUserRefCon__10TUMsgTokenFPUl
*/
    JT _ZN10TUMsgToken13GetUserRefConEPm, 0180111c

/*
C++: TUSharedMemMsg::GetUserRefCon(unsigned long*)
MPW-mangled: GetUserRefCon__14TUSharedMemMsgFPUl
*/
    JT _ZN14TUSharedMemMsg13GetUserRefConEPm, 01801120

/*
C++: TCardSocket::GetVPCPins()
MPW-mangled: GetVPCPins__11TCardSocketFv
*/
    JT _ZN11TCardSocket10GetVPCPinsEv, 01802df4

/*
C++: TView::GetValue(const RefVar&, const RefVar&)
MPW-mangled: GetValue__5TViewFRC6RefVarT1
*/
    JT GetValue__5TViewFRC6RefVarT1, 01800c20

/*
C++: TNSDebugAPI::GetVar(long, long)
MPW-mangled: GetVar__11TNSDebugAPIFlT1
*/
    JT GetVar__11TNSDebugAPIFlT1, 01802818

/*
C++: TView::GetVar(const RefVar&) const
MPW-mangled: GetVar__5TViewCFRC6RefVar
*/
    JT GetVar__5TViewCFRC6RefVar, 01800c24

/*
C++: GetVariable(const RefVar&, const RefVar&, long*, int)
MPW-mangled: GetVariable__FRC6RefVarT1Pli
*/
    JT GetVariable__FRC6RefVarT1Pli, 01800940

/*
C++: GetVccTimeout(int)
MPW-mangled: GetVccTimeout__Fi
*/
    JT _Z13GetVccTimeouti, 018027ac

/*
C++: TPackageIterator::GetVersion()
MPW-mangled: GetVersion__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator10GetVersionEv, 01801124

/*
C++: GetView(const RefVar&)
MPW-mangled: GetView__FRC6RefVar
*/
    JT _Z7GetViewRK6RefVar, 01800c28

/*
C++: GetView(const RefVar&, const RefVar&)
MPW-mangled: GetView__FRC6RefVarT1
*/
    JT GetView__FRC6RefVarT1, 01800c2c

/*
C++: TView::GetWindowView()
MPW-mangled: GetWindowView__5TViewFv
*/
    JT _ZN5TView13GetWindowViewEv, 01800c30
    JT GetWiredHeap, 01801678

/*
C++: TWRecognizer::GetWordString(TWRecUnit*, unsigned long)
MPW-mangled: GetWordString__12TWRecognizerFP9TWRecUnitUl
*/
    JT _ZN12TWRecognizer13GetWordStringEP9TWRecUnitm, 01802d38

/*
C++: TView::GetWriteableProtoVariable(const RefVar&)
MPW-mangled: GetWriteableProtoVariable__5TViewFRC6RefVar
*/
    JT _ZN5TView25GetWriteableProtoVariableERK6RefVar, 01800c34

/*
C++: TView::GetWriteableVariable(const RefVar&)
MPW-mangled: GetWriteableVariable__5TViewFRC6RefVar
*/
    JT _ZN5TView20GetWriteableVariableERK6RefVar, 01800c38

/*
C++: CBufferList::Get()
MPW-mangled: Get__11CBufferListFv
*/
    JT _ZN11CBufferList3GetEv, 01801ad8

/*
C++: CBufferPipe::Get()
MPW-mangled: Get__11CBufferPipeFv
*/
    JT _ZN11CBufferPipe3GetEv, 01801adc

/*
C++: CRingBuffer::Get()
MPW-mangled: Get__11CRingBufferFv
*/
    JT _ZN11CRingBuffer3GetEv, 01801ae0

/*
C++: static TUPageManager::Get(unsigned long&, unsigned long, int)
MPW-mangled: Get__13TUPageManagerSFRUlUli
*/
    JT _ZN13TUPageManager3GetERmmi, 0180103c

/*
C++: CBufferSegment::Get()
MPW-mangled: Get__14CBufferSegmentFv
*/
    JT _ZN14CBufferSegment3GetEv, 01801ae4

/*
C++: TStoreHashTable::Get(long, char*, long*)
MPW-mangled: Get__15TStoreHashTableFlPcPl
*/
    JT _ZN15TStoreHashTable3GetElPcPl, 0180091c

/*
C++: TUDomainManager::Get(unsigned long&, int)
MPW-mangled: Get__15TUDomainManagerFRUli
*/
    JT _ZN15TUDomainManager3GetERmi, 01801040

/*
C++: CShadowRingBuffer::Get()
MPW-mangled: Get__17CShadowRingBufferFv
*/
    JT _ZN17CShadowRingBuffer3GetEv, 01801ae8

/*
C++: CShadowBufferSegment::Get()
MPW-mangled: Get__20CShadowBufferSegmentFv
*/
    JT _ZN20CShadowBufferSegment3GetEv, 01801aec

/*
C++: CBufferList::Getn(unsigned char*, long)
MPW-mangled: Getn__11CBufferListFPUcl
*/
    JT _ZN11CBufferList4GetnEPhl, 01801b0c

/*
C++: CRingBuffer::Getn(unsigned char*, long)
MPW-mangled: Getn__11CRingBufferFPUcl
*/
    JT _ZN11CRingBuffer4GetnEPhl, 01801b10

/*
C++: CBufferSegment::Getn(unsigned char*, long)
MPW-mangled: Getn__14CBufferSegmentFPUcl
*/
    JT _ZN14CBufferSegment4GetnEPhl, 01801b14

/*
C++: CShadowRingBuffer::Getn(unsigned char*, long)
MPW-mangled: Getn__17CShadowRingBufferFPUcl
*/
    JT _ZN17CShadowRingBuffer4GetnEPhl, 01801b18

/*
C++: CShadowBufferSegment::Getn(unsigned char*, long)
MPW-mangled: Getn__20CShadowBufferSegmentFPUcl
*/
    JT _ZN20CShadowBufferSegment4GetnEPhl, 01801b1c

/*
C++: GiveObject(unsigned long, unsigned long)
MPW-mangled: GiveObject__FUlT1
*/
    JT GiveObject__FUlT1, 01801128
    JT HGetState, 018017d4
    JT HLock, 018017d8
    JT HSetState, 018017dc
    JT HUnlock, 018017e0
    JT HandToHand, 018017c8

/*
C++: TEndpointEventHandler::HandleAborts(unsigned char)
MPW-mangled: HandleAborts__21TEndpointEventHandlerFUc
*/
    JT _ZN21TEndpointEventHandler12HandleAbortsEh, 0180032c

/*
C++: TEndpoint::HandleComplete(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: HandleComplete__9TEndpointFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN9TEndpoint14HandleCompleteEP10TUMsgTokenPmP7TAEvent, 01800330

/*
C++: TEndpoint::HandleEvent(unsigned long, TAEvent*, unsigned long)
MPW-mangled: HandleEvent__9TEndpointFUlP7TAEventT1
*/
    JT HandleEvent__9TEndpointFUlP7TAEventT1, 01800334

/*
C++: TCommTool::HandleReply(unsigned long, unsigned long)
MPW-mangled: HandleReply__9TCommToolFUlT1
*/
    JT HandleReply__9TCommToolFUlT1, 018006c4

/*
C++: TCommTool::HandleRequest(TUMsgToken&, unsigned long)
MPW-mangled: HandleRequest__9TCommToolFR10TUMsgTokenUl
*/
    JT _ZN9TCommTool13HandleRequestER10TUMsgTokenm, 018006c8
    JT HandleToHeap, 0180167c

/*
C++: HasChars(unsigned short*)
MPW-mangled: HasChars__FPUs
*/
    JT _Z8HasCharsPt, 01802b60

/*
C++: HasDigits(unsigned short*)
MPW-mangled: HasDigits__FPUs
*/
    JT _Z9HasDigitsPt, 01802b64

/*
C++: TUEnvironment::HasDomain(unsigned long, unsigned char*, unsigned char*)
MPW-mangled: HasDomain__13TUEnvironmentFUlPUcT2
*/
    JT HasDomain__13TUEnvironmentFUlPUcT2, 0180112c

/*
C++: HasSpaces(unsigned short*)
MPW-mangled: HasSpaces__FPUs
*/
    JT _Z9HasSpacesPt, 01802b68

/*
C++: TView::HasVisRgn() const
MPW-mangled: HasVisRgn__5TViewCFv
*/
    JT HasVisRgn__5TViewCFv, 01800c3c
    JT HashCallChain, 018017cc
    JT HashCallChainWithStack, 018017d0

/*
C++: HeapBounds(char**, char**)
MPW-mangled: HeapBounds__FPPcT1
*/
    JT HeapBounds__FPPcT1, 018027cc
    JT HeapSeed, 01801680

/*
C++: TUCTTable::HeldBy(TTask*, unsigned long)
MPW-mangled: HeldBy__9TUCTTableFP5TTaskUl
*/
    JT _ZN9TUCTTable6HeldByEP5TTaskm, 01801130

/*
C++: CBufferList::Hide(long, int)
MPW-mangled: Hide__11CBufferListFli
*/
    JT _ZN11CBufferList4HideEli, 01801b3c

/*
C++: CBufferSegment::Hide(long, int)
MPW-mangled: Hide__14CBufferSegmentFli
*/
    JT _ZN14CBufferSegment4HideEli, 01801b40

/*
C++: CShadowBufferSegment::Hide(long, int)
MPW-mangled: Hide__20CShadowBufferSegmentFli
*/
    JT _ZN20CShadowBufferSegment4HideEli, 01801b44

/*
C++: TView::Hide()
MPW-mangled: Hide__5TViewFv
*/
    JT _ZN5TView4HideEv, 01800c40

/*
C++: TView::Hilite(unsigned char)
MPW-mangled: Hilite__5TViewFUc
*/
    JT _ZN5TView6HiliteEh, 01800c44

/*
C++: TCommTool::HoldAbort()
MPW-mangled: HoldAbort__9TCommToolFv
*/
    JT _ZN9TCommTool9HoldAbortEv, 018006cc

/*
C++: HoldSchedule()
MPW-mangled: HoldSchedule__Fv
*/
    JT _Z12HoldSchedulev, 01801134

/*
C++: TCardSocket::IOBaseAddr()
MPW-mangled: IOBaseAddr__11TCardSocketFv
*/
    JT _ZN11TCardSocket10IOBaseAddrEv, 018011d0

/*
C++: IOPowerOff(unsigned long)
MPW-mangled: IOPowerOff__FUl
*/
    JT _Z10IOPowerOffm, 01802cb8

/*
C++: IOPowerOn(unsigned long)
MPW-mangled: IOPowerOn__FUl
*/
    JT _Z9IOPowerOnm, 01802cbc

/*
C++: ISREAL(long)
MPW-mangled: ISREAL__Fl
*/
    JT _Z6ISREALl, 01800960

/*
C++: IdToStore(unsigned long, TStore**, unsigned long*)
MPW-mangled: IdToStore__FUlPP6TStorePUl
*/
    JT _Z9IdToStoremPP6TStorePm, 01801138

/*
C++: IdToVAddr(unsigned long, unsigned long*)
MPW-mangled: IdToVAddr__FUlPUl
*/
    JT _Z9IdToVAddrmPm, 0180113c

/*
C++: TATA::IdentifyDrive(TATADriveInfo*, unsigned char)
MPW-mangled: IdentifyDrive__4TATAFP13TATADriveInfoUc
*/
    JT _ZN4TATA13IdentifyDriveEP13TATADriveInfoh, 01802a8c

/*
C++: TAEventHandler::IdleProc(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: IdleProc__14TAEventHandlerFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN14TAEventHandler8IdleProcEP10TUMsgTokenPmP7TAEvent, 01801b48

/*
C++: TEndpointEventHandler::IdleProc(TUMsgToken*, unsigned long*, TAEvent*)
MPW-mangled: IdleProc__21TEndpointEventHandlerFP10TUMsgTokenPUlP7TAEvent
*/
    JT _ZN21TEndpointEventHandler8IdleProcEP10TUMsgTokenPmP7TAEvent, 01800338

/*
C++: TView::Idle(long)
MPW-mangled: Idle__5TViewFl
*/
    JT _ZN5TView4IdleEl, 01800c48

/*
C++: TClassInfo::ImplementationName() const
MPW-mangled: ImplementationName__10TClassInfoCFv
*/
    JT ImplementationName__10TClassInfoCFv, 01801140

/*
C++: TNSDebugAPI::Implementor(long)
MPW-mangled: Implementor__11TNSDebugAPIFl
*/
    JT _ZN11TNSDebugAPI11ImplementorEl, 0180280c

/*
C++: TCommTool::ImportConnectPB(TCommToolConnectRequest*)
MPW-mangled: ImportConnectPB__9TCommToolFP23TCommToolConnectRequest
*/
    JT _ZN9TCommTool15ImportConnectPBEP23TCommToolConnectRequest, 018006d0

/*
C++: IncrementCurrentStackPos()
MPW-mangled: IncrementCurrentStackPos__Fv
*/
    JT _Z24IncrementCurrentStackPosv, 01802490

/*
C++: TDate::IncrementMonth(long)
MPW-mangled: IncrementMonth__5TDateFl
*/
    JT _ZN5TDate14IncrementMonthEl, 01802618

/*
C++: TCommTool::InitAsyncRPCMsg(TUAsyncMessage&, unsigned long)
MPW-mangled: InitAsyncRPCMsg__9TCommToolFR14TUAsyncMessageUl
*/
    JT _ZN9TCommTool15InitAsyncRPCMsgER14TUAsyncMessagem, 018006d4

/*
C++: TEndpoint::InitBaseEndpoint(TEndpointEventHandler*)
MPW-mangled: InitBaseEndpoint__9TEndpointFP21TEndpointEventHandler
*/
    JT _ZN9TEndpoint16InitBaseEndpointEP21TEndpointEventHandler, 01800368

/*
C++: CArrayIterator::InitBounds(long, long, unsigned char)
MPW-mangled: InitBounds__14CArrayIteratorFlT1Uc
*/
    JT InitBounds__14CArrayIteratorFlT1Uc, 01801ba4

/*
C++: TOptionIterator::InitBounds(long, long)
MPW-mangled: InitBounds__15TOptionIteratorFlT1
*/
    JT InitBounds__15TOptionIteratorFlT1, 0180036c

/*
C++: TATA::InitDriveParam(unsigned char, unsigned char, unsigned char)
MPW-mangled: InitDriveParam__4TATAFUcN21
*/
    JT InitDriveParam__4TATAFUcN21, 01802a90

/*
C++: TPackageIterator::InitFields()
MPW-mangled: InitFields__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator10InitFieldsEv, 018011ac

/*
C++: TAEventHandler::InitIdler(unsigned long, TimeUnits, unsigned long, unsigned char)
MPW-mangled: InitIdler__14TAEventHandlerFUl9TimeUnitsT1Uc
*/
    JT InitIdler__14TAEventHandlerFUl9TimeUnitsT1Uc, 01801ba8

/*
C++: InitKMEMode(unsigned long, unsigned long, unsigned long)
MPW-mangled: InitKMEMode__FUlN21
*/
    JT InitKMEMode__FUlN21, 0180279c
    JT InitSerialDebugging, 01800034

/*
C++: TDate::InitWithMinutes(unsigned long)
MPW-mangled: InitWithMinutes__5TDateFUl
*/
    JT _ZN5TDate15InitWithMinutesEm, 0180261c

/*
C++: TDate::InitWithSeconds(unsigned long)
MPW-mangled: InitWithSeconds__5TDateFUl
*/
    JT _ZN5TDate15InitWithSecondsEm, 01802664

/*
C++: CMemObject::Init(unsigned long, void*, unsigned char, unsigned long)
MPW-mangled: Init__10CMemObjectFUlPvUcT1
*/
    JT Init__10CMemObjectFUlPvUcT1, 01801b4c

/*
C++: TIdleTimer::Init(unsigned long, TimeUnits, unsigned long, unsigned long)
MPW-mangled: Init__10TIdleTimerFUl9TimeUnitsN21
*/
    JT Init__10TIdleTimerFUl9TimeUnitsN21, 01801b50

/*
C++: TStartInfo::Init(TUMsgToken*, TCMEvent*)
MPW-mangled: Init__10TStartInfoFP10TUMsgTokenP8TCMEvent
*/
    JT _ZN10TStartInfo4InitEP10TUMsgTokenP8TCMEvent, 018020b8

/*
C++: CBufferList::Init(CList*, unsigned char)
MPW-mangled: Init__11CBufferListFP5CListUc
*/
    JT _ZN11CBufferList4InitEP5CListh, 01801b54

/*
C++: CBufferList::Init(unsigned char)
MPW-mangled: Init__11CBufferListFUc
*/
    JT _ZN11CBufferList4InitEh, 01801b58

/*
C++: CBufferPipe::Init(CBufferSegment*, CBufferSegment*, unsigned char)
MPW-mangled: Init__11CBufferPipeFP14CBufferSegmentT1Uc
*/
    JT Init__11CBufferPipeFP14CBufferSegmentT1Uc, 01801b60

/*
C++: CBufferPipe::Init(long, long)
MPW-mangled: Init__11CBufferPipeFlT1
*/
    JT Init__11CBufferPipeFlT1, 01801b5c

/*
C++: CRingBuffer::Init(void*, long, unsigned char, long, long)
MPW-mangled: Init__11CRingBufferFPvlUcN22
*/
    JT Init__11CRingBufferFPvlUcN22, 01801b68

/*
C++: CRingBuffer::Init(long)
MPW-mangled: Init__11CRingBufferFl
*/
    JT _ZN11CRingBuffer4InitEl, 01801b64

/*
C++: TPMIterator::Init()
MPW-mangled: Init__11TPMIteratorFv
*/
    JT _ZN11TPMIterator4InitEv, 01801144

/*
C++: TUSharedMem::Init()
MPW-mangled: Init__11TUSharedMemFv
*/
    JT _ZN11TUSharedMem4InitEv, 01801148

/*
C++: TOptionArray::Init(TSubArrayOption*)
MPW-mangled: Init__12TOptionArrayFP15TSubArrayOption
*/
    JT _ZN12TOptionArray4InitEP15TSubArrayOption, 0180033c

/*
C++: TOptionArray::Init(unsigned long)
MPW-mangled: Init__12TOptionArrayFUl
*/
    JT _ZN12TOptionArray4InitEm, 01800340

/*
C++: TOptionArray::Init(unsigned long, unsigned long)
MPW-mangled: Init__12TOptionArrayFUlT1
*/
    JT Init__12TOptionArrayFUlT1, 01800344

/*
C++: TOptionArray::Init()
MPW-mangled: Init__12TOptionArrayFv
*/
    JT _ZN12TOptionArray4InitEv, 01800348

/*
C++: TPartHandler::Init(unsigned long)
MPW-mangled: Init__12TPartHandlerFUl
*/
    JT _ZN12TPartHandler4InitEm, 0180114c

/*
C++: RealTimeAlarm::Init(unsigned long, unsigned long, unsigned long, void*, long, unsigned long)
MPW-mangled: Init__13RealTimeAlarmFUlN21PvlT1
*/
    JT Init__13RealTimeAlarmFUlN21PvlT1, 01801150

/*
C++: TEndpointPipe::Init(TEndpoint*, long, long, unsigned long, unsigned char)
MPW-mangled: Init__13TEndpointPipeFP9TEndpointlT2UlUc
*/
    JT Init__13TEndpointPipeFP9TEndpointlT2UlUc, 0180034c

/*
C++: TEndpointPipe::Init(TEndpoint*, long, long, unsigned long, unsigned char, PipeCallBack*)
MPW-mangled: Init__13TEndpointPipeFP9TEndpointlT2UlUcP12PipeCallBack
*/
    JT Init__13TEndpointPipeFP9TEndpointlT2UlUcP12PipeCallBack, 018025e8

/*
C++: TMemoryObject::Init(unsigned long, unsigned char, unsigned long)
MPW-mangled: Init__13TMemoryObjectFUlUcT1
*/
    JT Init__13TMemoryObjectFUlUcT1, 01802084

/*
C++: TUEnvironment::Init(void*)
MPW-mangled: Init__13TUEnvironmentFPv
*/
    JT _ZN13TUEnvironment4InitEPv, 01801154

/*
C++: CArrayIterator::Init(CDynamicArray*, long, long, unsigned char)
MPW-mangled: Init__14CArrayIteratorFP13CDynamicArraylT2Uc
*/
    JT Init__14CArrayIteratorFP13CDynamicArraylT2Uc, 01801b6c

/*
C++: CArrayIterator::Init()
MPW-mangled: Init__14CArrayIteratorFv
*/
    JT _ZN14CArrayIterator4InitEv, 01801b70

/*
C++: CBufferSegment::Init(void*, long, unsigned char, long, long)
MPW-mangled: Init__14CBufferSegmentFPvlUcN22
*/
    JT _ZN14CBufferSegment4InitEPvlhll, 01801b78

/*
C++: CBufferSegment::Init(long)
MPW-mangled: Init__14CBufferSegmentFl
*/
    JT _ZN14CBufferSegment4InitEl, 01801b74

/*
C++: TAEventHandler::Init(unsigned long, unsigned long)
MPW-mangled: Init__14TAEventHandlerFUlT1
*/
    JT _ZN14TAEventHandler4InitEmm, 01801b7c

/*
C++: TAppWorldState::Init(unsigned long)
MPW-mangled: Init__14TAppWorldStateFUl
*/
    JT _ZN14TAppWorldState4InitEm, 01801b80

/*
C++: TAppWorldState::Init()
MPW-mangled: Init__14TAppWorldStateFv
*/
    JT _ZN14TAppWorldState4InitEv, 01801b84

/*
C++: static TRealTimeClock::Init()
MPW-mangled: Init__14TRealTimeClockSFv
*/
    JT _ZN14TRealTimeClock4InitEv, 0180115c

/*
C++: TUAsyncMessage::Init(unsigned char)
MPW-mangled: Init__14TUAsyncMessageFUc
*/
    JT _ZN14TUAsyncMessage4InitEh, 01801160

/*
C++: TUSharedMemMsg::Init()
MPW-mangled: Init__14TUSharedMemMsgFv
*/
    JT _ZN14TUSharedMemMsg4InitEv, 01801164

/*
C++: TEndpointClient::Init(TEndpoint*, unsigned long, unsigned long)
MPW-mangled: Init__15TEndpointClientFP9TEndpointUlT2
*/
    JT Init__15TEndpointClientFP9TEndpointUlT2, 01800350

/*
C++: TEzEndpointPipe::Init(ConnectionType, char**, unsigned long)
MPW-mangled: Init__15TEzEndpointPipeF14ConnectionTypePPcUl
*/
    JT _ZN15TEzEndpointPipe4InitE14ConnectionTypePPcm, 01800354

/*
C++: TOptionIterator::Init(TOptionArray*, long, long)
MPW-mangled: Init__15TOptionIteratorFP12TOptionArraylT2
*/
    JT Init__15TOptionIteratorFP12TOptionArraylT2, 01800358

/*
C++: TUDomainManager::Init(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long)
MPW-mangled: Init__15TUDomainManagerFUlN41
*/
    JT Init__15TUDomainManagerFUlN41, 01801168

/*
C++: TURdWrSemaphore::Init()
MPW-mangled: Init__15TURdWrSemaphoreFv
*/
    JT _ZN15TURdWrSemaphore4InitEv, 0180116c

/*
C++: TCPNewtonIdTuple::Init()
MPW-mangled: Init__16TCPNewtonIdTupleFv
*/
    JT _ZN16TCPNewtonIdTuple4InitEv, 0180035c

/*
C++: TCardCISIterator::Init(TCardSocket*)
MPW-mangled: Init__16TCardCISIteratorFP11TCardSocket
*/
    JT _ZN16TCardCISIterator4InitEP11TCardSocket, 018024f4

/*
C++: TPackageIterator::Init()
MPW-mangled: Init__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator4InitEv, 01801170

/*
C++: TSendSystemEvent::Init()
MPW-mangled: Init__16TSendSystemEventFv
*/
    JT _ZN16TSendSystemEvent4InitEv, 01801174

/*
C++: TUSemaphoreGroup::Init(unsigned long)
MPW-mangled: Init__16TUSemaphoreGroupFUl
*/
    JT _ZN16TUSemaphoreGroup4InitEm, 01801178

/*
C++: CShadowRingBuffer::Init(unsigned long, long, long)
MPW-mangled: Init__17CShadowRingBufferFUllT2
*/
    JT Init__17CShadowRingBufferFUllT2, 01801b88

/*
C++: TDoubleQContainer::Init(char*)
MPW-mangled: Init__17TDoubleQContainerFPc
*/
    JT _ZN17TDoubleQContainer4InitEPc, 0180117c

/*
C++: TUSemaphoreOpList::Init(unsigned long, ...)
MPW-mangled: Init__17TUSemaphoreOpListFUle
*/
    JT _ZN17TUSemaphoreOpList4InitEmz, 01801180

/*
C++: TUserAbortHandler::Init(unsigned long, unsigned long)
MPW-mangled: Init__17TUserAbortHandlerFUlT1
*/
    JT Init__17TUserAbortHandlerFUlT1, 01800360

/*
C++: TULockingSemaphore::Init()
MPW-mangled: Init__18TULockingSemaphoreFv
*/
    JT _ZN18TULockingSemaphore4InitEv, 01801184

/*
C++: TOSErrorAlertDialog::Init(unsigned short*, unsigned short*)
MPW-mangled: Init__19TOSErrorAlertDialogFPUsT1
*/
    JT Init__19TOSErrorAlertDialogFPUsT1, 01802798

/*
C++: TSystemEventHandler::Init(unsigned long, unsigned long)
MPW-mangled: Init__19TSystemEventHandlerFUlT1
*/
    JT Init__19TSystemEventHandlerFUlT1, 01801b8c

/*
C++: CShadowBufferSegment::Init(unsigned long, long, long)
MPW-mangled: Init__20CShadowBufferSegmentFUllT2
*/
    JT Init__20CShadowBufferSegmentFUllT2, 01801b90

/*
C++: TEndpointEventHandler::Init(unsigned long, unsigned long, unsigned long)
MPW-mangled: Init__21TEndpointEventHandlerFUlN21
*/
    JT Init__21TEndpointEventHandlerFUlN21, 01800364

/*
C++: TUPhys::Init(unsigned long, unsigned long, unsigned char, unsigned char)
MPW-mangled: Init__6TUPhysFUlT1UcT3
*/
    JT Init__6TUPhysFUlT1UcT3, 01801188

/*
C++: TUPort::Init()
MPW-mangled: Init__6TUPortFv
*/
    JT _ZN6TUPort4InitEv, 0180118c

/*
C++: TUTask::Init(void (*)(void*, unsigned long, unsigned long), unsigned long, unsigned long, void*, unsigned long, unsigned long)
MPW-mangled: Init__6TUTaskFPFPvUlT2_vUlT2PvN22
*/
    JT _ZN6TUTask4InitEPFvPvmmEmmS0_mm, 01801190

/*
C++: TUTask::Init(void (*)(void*, unsigned long, unsigned long), unsigned long, unsigned long, void*, unsigned long, unsigned long, unsigned long)
MPW-mangled: Init__6TUTaskFPFPvUlT2_vUlT2PvN32
*/
    JT Init__6TUTaskFPFPvUlT2_vUlT2PvN32, 01801194

/*
C++: TUDomain::Init(unsigned long, unsigned long, unsigned long)
MPW-mangled: Init__8TUDomainFUlN21
*/
    JT Init__8TUDomainFUlN21, 01801198

/*
C++: CPartPipe::Init(unsigned long, CShadowRingBuffer*, unsigned char)
MPW-mangled: Init__9CPartPipeFUlP17CShadowRingBufferUc
*/
    JT _ZN9CPartPipe4InitEmP17CShadowRingBufferh, 0180119c

/*
C++: CRingPipe::Init(CBaseRingBuffer*, unsigned char)
MPW-mangled: Init__9CRingPipeFP15CBaseRingBufferUc
*/
    JT _ZN9CRingPipe4InitEP15CBaseRingBufferh, 01801b98

/*
C++: CRingPipe::Init(long)
MPW-mangled: Init__9CRingPipeFl
*/
    JT _ZN9CRingPipe4InitEl, 01801b94

/*
C++: CTaskPipe::Init(unsigned long, CBaseRingBuffer*, unsigned char)
MPW-mangled: Init__9CTaskPipeFUlP15CBaseRingBufferUc
*/
    JT _ZN9CTaskPipe4InitEmP15CBaseRingBufferh, 018011a4

/*
C++: CTaskPipe::Init(unsigned long, long)
MPW-mangled: Init__9CTaskPipeFUll
*/
    JT _ZN9CTaskPipe4InitEml, 018011a0

/*
C++: TAppWorld::Init(unsigned long, unsigned char, unsigned long, unsigned long, unsigned long)
MPW-mangled: Init__9TAppWorldFUlUcN31
*/
    JT Init__9TAppWorldFUlUcN31, 01801b9c

/*
C++: TAppWorld::Init(unsigned long, unsigned char, unsigned long)
MPW-mangled: Init__9TAppWorldFUlUcT1
*/
    JT Init__9TAppWorldFUlUcT1, 01801ba0

/*
C++: TUMonitor::Init(void (*)(void*, unsigned long, void*), unsigned long, void*, unsigned long, unsigned char, unsigned long, unsigned char)
MPW-mangled: Init__9TUMonitorFPFPvUlT1_vUlPvT2UcT2T5
*/
    JT Init__9TUMonitorFPFPvUlT1_vUlPvT2UcT2T5, 018011a8

/*
C++: TATA::Initialize(TCardSocket*, TCardPCMCIA*, unsigned long)
MPW-mangled: Initialize__4TATAFP11TCardSocketP11TCardPCMCIAUl
*/
    JT _ZN4TATA10InitializeEP11TCardSocketP11TCardPCMCIAm, 01802a94

/*
C++: CBufferList::InsertAt(long, CBuffer*)
MPW-mangled: InsertAt__11CBufferListFlP7CBuffer
*/
    JT _ZN11CBufferList8InsertAtElP7CBuffer, 01801bb4

/*
C++: CList::InsertAt(long, void*)
MPW-mangled: InsertAt__5CListFlPv
*/
    JT _ZN5CList8InsertAtElPv, 01801bb8

/*
C++: CBufferList::InsertBefore(long, CBuffer*)
MPW-mangled: InsertBefore__11CBufferListFlP7CBuffer
*/
    JT _ZN11CBufferList12InsertBeforeElP7CBuffer, 01801bbc

/*
C++: CSortedList::InsertDuplicate(long, void*, void*)
MPW-mangled: InsertDuplicate__11CSortedListFlPvT2
*/
    JT InsertDuplicate__11CSortedListFlPvT2, 01801bc0

/*
C++: CDynamicArray::InsertElementsBefore(long, void*, long)
MPW-mangled: InsertElementsBefore__13CDynamicArrayFlPvT1
*/
    JT InsertElementsBefore__13CDynamicArrayFlPvT1, 01801bc4

/*
C++: CArrayIterator::InsertElementsBefore(long, long)
MPW-mangled: InsertElementsBefore__14CArrayIteratorFlT1
*/
    JT InsertElementsBefore__14CArrayIteratorFlT1, 01801bc8

/*
C++: CBufferList::InsertFirst(CBuffer*)
MPW-mangled: InsertFirst__11CBufferListFP7CBuffer
*/
    JT _ZN11CBufferList11InsertFirstEP7CBuffer, 01801bcc

/*
C++: CBufferList::InsertLast(CBuffer*)
MPW-mangled: InsertLast__11CBufferListFP7CBuffer
*/
    JT _ZN11CBufferList10InsertLastEP7CBuffer, 01801bd0

/*
C++: TOptionArray::InsertOptionAt(long, TOption*)
MPW-mangled: InsertOptionAt__12TOptionArrayFlP7TOption
*/
    JT _ZN12TOptionArray14InsertOptionAtElP7TOption, 01800370

/*
C++: TOptionArray::InsertSubArrayAt(long, TOptionArray*)
MPW-mangled: InsertSubArrayAt__12TOptionArrayFlP12TOptionArray
*/
    JT _ZN12TOptionArray16InsertSubArrayAtElP12TOptionArray, 01800374

/*
C++: CSortedList::InsertUnique(void*)
MPW-mangled: InsertUnique__11CSortedListFPv
*/
    JT _ZN11CSortedList12InsertUniqueEPv, 01801bd4

/*
C++: CList::InsertUnique(void*)
MPW-mangled: InsertUnique__5CListFPv
*/
    JT _ZN5CList12InsertUniqueEPv, 01801bd8

/*
C++: TOptionArray::InsertVarOptionAt(long, TOption*, void*, unsigned long)
MPW-mangled: InsertVarOptionAt__12TOptionArrayFlP7TOptionPvUl
*/
    JT _ZN12TOptionArray17InsertVarOptionAtElP7TOptionPvm, 01800378

/*
C++: CBufferList::Insert(CBuffer*)
MPW-mangled: Insert__11CBufferListFP7CBuffer
*/
    JT _ZN11CBufferList6InsertEP7CBuffer, 01801bac

/*
C++: CSortedList::Insert(void*)
MPW-mangled: Insert__11CSortedListFPv
*/
    JT _ZN11CSortedList6InsertEPv, 01801bb0

/*
C++: TStoreHashTable::Insert(unsigned long, char*, long)
MPW-mangled: Insert__15TStoreHashTableFUlPcl
*/
    JT _ZN15TStoreHashTable6InsertEmPcl, 01800944

/*
C++: TView::InsideView(TPoint&)
MPW-mangled: InsideView__5TViewFR6TPoint
*/
    JT _ZN5TView10InsideViewER6TPoint, 01800c4c

/*
C++: InstallFramesFunctionProfiler(void (*)(const RefVar&, const RefVar&, long, long, FramesProfilingKind, void*), void*)
MPW-mangled: InstallFramesFunctionProfiler__FPFRC6RefVarT1lT319FramesProfilingKindPv_vPv
*/
    JT InstallFramesFunctionProfiler__FPFRC6RefVarT1lT319FramesProfilingKindPv_vPv, 018020e8

/*
C++: InstallPackage(char*, SourceType, unsigned long*, unsigned char*, unsigned char*)
MPW-mangled: InstallPackage__FPc10SourceTypePUlPUcT4
*/
    JT InstallPackage__FPc10SourceTypePUlPUcT4, 018011b4

/*
C++: TPartHandler::Install(TPkPartInstallEvent*)
MPW-mangled: Install__12TPartHandlerFP19TPkPartInstallEvent
*/
    JT _ZN12TPartHandler7InstallEP19TPkPartInstallEvent, 018011b0

/*
C++: TFramePartHandler::Install(const PartId&, SourceType, PartInfo*)
MPW-mangled: Install__17TFramePartHandlerFRC6PartId10SourceTypeP8PartInfo
*/
    JT _ZN17TFramePartHandler7InstallERK6PartId10SourceTypeP8PartInfo, 01800948

/*
C++: IntegerString(long, unsigned short*)
MPW-mangled: IntegerString__FlPUs
*/
    JT _Z13IntegerStringlPt, 01801be0

/*
C++: TClassInfo::InterfaceName() const
MPW-mangled: InterfaceName__10TClassInfoCFv
*/
    JT InterfaceName__10TClassInfoCFv, 018011bc

/*
C++: Intern(char*)
MPW-mangled: Intern__FPc
*/
    JT _Z6InternPc, 0180094c
/* Also export const char* function to the same vector */
    JT _Z6InternPKc, 0180094c
    JT InternalRAMInfo, 0180276c
    JT InternalStoreInfo, 01802768

/*
C++: InterpretBlock(const RefVar&, const RefVar&)
MPW-mangled: InterpretBlock__FRC6RefVarT1
*/
    JT InterpretBlock__FRC6RefVarT1, 01800950

/*
C++: TWRecognizer::InterpretationCount(TWRecUnit*)
MPW-mangled: InterpretationCount__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer19InterpretationCountEP9TWRecUnit, 01802d3c

/*
C++: static TRealTimeClock::InterruptEntry()
MPW-mangled: InterruptEntry__14TRealTimeClockSFv
*/
    JT _ZN14TRealTimeClock14InterruptEntryEv, 018011c0

/*
C++: TAppWorld::InterruptHandler(unsigned long*, TAEvent*)
MPW-mangled: InterruptHandler__9TAppWorldFPUlP7TAEvent
*/
    JT _ZN9TAppWorld16InterruptHandlerEPmP7TAEvent, 01801be4

/*
C++: TCardSocket::InterruptState(TSocketInt)
MPW-mangled: InterruptState__11TCardSocketF10TSocketInt
*/
    JT _ZN11TCardSocket14InterruptStateE10TSocketInt, 01802a98

/*
C++: InvalidatePhys(unsigned long)
MPW-mangled: InvalidatePhys__FUl
*/
    JT _Z14InvalidatePhysm, 018011c8

/*
C++: InvalidatePhys(unsigned long, TPhys*)
MPW-mangled: InvalidatePhys__FUlP5TPhys
*/
    JT _Z14InvalidatePhysmP5TPhys, 018011cc

/*
C++: TWRecognizer::InvalidateUnit(TWRecUnit*)
MPW-mangled: InvalidateUnit__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer14InvalidateUnitEP9TWRecUnit, 01802d40

/*
C++: TUPhys::Invalidate()
MPW-mangled: Invalidate__6TUPhysFv
*/
    JT _ZN6TUPhys10InvalidateEv, 018011c4

/*
C++: IsAlphaNumeric(unsigned short)
MPW-mangled: IsAlphaNumeric__FUs
*/
    JT _Z14IsAlphaNumerict, 01801bec

/*
C++: IsAlphabet(unsigned short)
MPW-mangled: IsAlphabet__FUs
*/
    JT _Z10IsAlphabett, 01801be8

/*
C++: IsArray(const RefVar&)
MPW-mangled: IsArray__FRC6RefVar
*/
    JT _Z7IsArrayRK6RefVar, 01802058

/*
C++: IsBinary(const RefVar&)
MPW-mangled: IsBinary__FRC6RefVar
*/
    JT _Z8IsBinaryRK6RefVar, 0180298c

/*
C++: IsBreaker(unsigned short)
MPW-mangled: IsBreaker__FUs
*/
    JT _Z9IsBreakert, 01801bf0

/*
C++: TCardSocket::IsCardDetected()
MPW-mangled: IsCardDetected__11TCardSocketFv
*/
    JT _ZN11TCardSocket14IsCardDetectedEv, 01802a9c

/*
C++: IsChar(const RefVar&)
MPW-mangled: IsChar__FRC6RefVar
*/
    JT _Z6IsCharRK6RefVar, 01802990

/*
C++: TPMIterator::IsCopyProtected()
MPW-mangled: IsCopyProtected__11TPMIteratorFv
*/
    JT _ZN11TPMIterator15IsCopyProtectedEv, 018011d8
    JT IsDebuggerPresent, 01800038

/*
C++: IsDelimiter(unsigned short)
MPW-mangled: IsDelimiter__FUs
*/
    JT _Z11IsDelimitert, 01801bf4

/*
C++: IsDigit(unsigned short)
MPW-mangled: IsDigit__FUs
*/
    JT _Z7IsDigitt, 01801bf8

/*
C++: CRingBuffer::IsEmpty() const
MPW-mangled: IsEmpty__11CRingBufferCFv
*/
    JT IsEmpty__11CRingBufferCFv, 01801bfc

/*
C++: CShadowRingBuffer::IsEmpty() const
MPW-mangled: IsEmpty__17CShadowRingBufferCFv
*/
    JT IsEmpty__17CShadowRingBufferCFv, 01801c00
    JT IsFakeHandle, 018017e4
    JT IsFakeIndirectBlock, 01801684

/*
C++: IsFaultBlock(long)
MPW-mangled: IsFaultBlock__Fl
*/
    JT _Z12IsFaultBlockl, 01800954

/*
C++: IsFrame(const RefVar&)
MPW-mangled: IsFrame__FRC6RefVar
*/
    JT _Z7IsFrameRK6RefVar, 0180205c

/*
C++: CRingBuffer::IsFull() const
MPW-mangled: IsFull__11CRingBufferCFv
*/
    JT IsFull__11CRingBufferCFv, 01801c04

/*
C++: CShadowRingBuffer::IsFull() const
MPW-mangled: IsFull__17CShadowRingBufferCFv
*/
    JT IsFull__17CShadowRingBufferCFv, 01801c08

/*
C++: IsFunction(const RefVar&)
MPW-mangled: IsFunction__FRC6RefVar
*/
    JT _Z10IsFunctionRK6RefVar, 01800958

/*
C++: IsHandlerPresent(unsigned long)
MPW-mangled: IsHandlerPresent__FUl
*/
    JT _Z16IsHandlerPresentm, 01802bd8

/*
C++: TCardSocket::IsIOInteface()
MPW-mangled: IsIOInteface__11TCardSocketFv
*/
    JT _ZN11TCardSocket12IsIOIntefaceEv, 018011dc

/*
C++: TCardSocket::IsIRQ()
MPW-mangled: IsIRQ__11TCardSocketFv
*/
    JT _ZN11TCardSocket5IsIRQEv, 018011e0

/*
C++: IsInstance(const RefVar&, const RefVar&)
MPW-mangled: IsInstance__FRC6RefVarT1
*/
    JT IsInstance__FRC6RefVarT1, 0180095c

/*
C++: IsInt(const RefVar&)
MPW-mangled: IsInt__FRC6RefVar
*/
    JT _Z5IsIntRK6RefVar, 01802994

/*
C++: IsLargeBinary(const RefVar&)
MPW-mangled: IsLargeBinary__FRC6RefVar
*/
    JT _Z13IsLargeBinaryRK6RefVar, 01802ccc

/*
C++: IsMagicPtr(const RefVar&)
MPW-mangled: IsMagicPtr__FRC6RefVar
*/
    JT _Z10IsMagicPtrRK6RefVar, 01802998

/*
C++: IsNativeFunction(const RefVar&)
MPW-mangled: IsNativeFunction__FRC6RefVar
*/
    JT _Z16IsNativeFunctionRK6RefVar, 018027e8

/*
C++: IsNumber(const RefVar&)
MPW-mangled: IsNumber__FRC6RefVar
*/
    JT _Z8IsNumberRK6RefVar, 0180299c

/*
C++: TCardSocket::IsPCMCIABusEnable()
MPW-mangled: IsPCMCIABusEnable__11TCardSocketFv
*/
    JT _ZN11TCardSocket17IsPCMCIABusEnableEv, 018011e8

/*
C++: TCardSocket::IsPCMCIABus()
MPW-mangled: IsPCMCIABus__11TCardSocketFv
*/
    JT _ZN11TCardSocket11IsPCMCIABusEv, 018011e4

/*
C++: IsPackageHeader(unsigned long, unsigned long)
MPW-mangled: IsPackageHeader__FUlT1
*/
    JT IsPackageHeader__FUlT1, 01802cc0

/*
C++: IsPackage(const RefVar&)
MPW-mangled: IsPackage__FRC6RefVar
*/
    JT _Z9IsPackageRK6RefVar, 01802cd0

/*
C++: IsPtr(const RefVar&)
MPW-mangled: IsPtr__FRC6RefVar
*/
    JT _Z5IsPtrRK6RefVar, 018029a0

/*
C++: IsPunctSymbol(unsigned short*, long)
MPW-mangled: IsPunctSymbol__FPUsl
*/
    JT _Z13IsPunctSymbolPtl, 01802b6c

/*
C++: IsReadOnly(const RefVar&)
MPW-mangled: IsReadOnly__FRC6RefVar
*/
    JT _Z10IsReadOnlyRK6RefVar, 018029a4

/*
C++: TCardSocket::IsReady()
MPW-mangled: IsReady__11TCardSocketFv
*/
    JT _ZN11TCardSocket7IsReadyEv, 018011ec

/*
C++: IsRealPtr(const RefVar&)
MPW-mangled: IsRealPtr__FRC6RefVar
*/
    JT _Z9IsRealPtrRK6RefVar, 018029a8

/*
C++: IsReal(const RefVar&)
MPW-mangled: IsReal__FRC6RefVar
*/
    JT _Z6IsRealRK6RefVar, 018029ac

/*
C++: TClassInfoRegistry::IsRegistered(const TClassInfo*, unsigned char) const
MPW-mangled: IsRegistered__18TClassInfoRegistryCFPC10TClassInfoUc
*/
    JT IsRegistered__18TClassInfoRegistryCFPC10TClassInfoUc, 018011f0

/*
C++: IsReturn(unsigned short)
MPW-mangled: IsReturn__FUs
*/
    JT _Z8IsReturnt, 01801c0c

/*
C++: IsSafeHeap(void*)
MPW-mangled: IsSafeHeap__FPv
*/
    JT _Z10IsSafeHeapPv, 018011f4

/*
C++: TInterpreter::IsSend()
MPW-mangled: IsSend__12TInterpreterFv
*/
    JT _ZN12TInterpreter6IsSendEv, 0180273c
    JT IsSkiaHeap, 01801688

/*
C++: IsSoupEntry(const RefVar&)
MPW-mangled: IsSoupEntry__FRC6RefVar
*/
    JT _Z11IsSoupEntryRK6RefVar, 01800964

/*
C++: IsSpace(unsigned short)
MPW-mangled: IsSpace__FUs
*/
    JT _Z7IsSpacet, 01801c10

/*
C++: TCardSocket::IsStatusChanged()
MPW-mangled: IsStatusChanged__11TCardSocketFv
*/
    JT _ZN11TCardSocket15IsStatusChangedEv, 018011f8

/*
C++: IsString(const RefVar&)
MPW-mangled: IsString__FRC6RefVar
*/
    JT _Z8IsStringRK6RefVar, 01800968

/*
C++: IsSubclassRef(long, long)
MPW-mangled: IsSubclassRef__FlT1
*/
    JT IsSubclassRef__FlT1, 0180096c

/*
C++: IsSubclass(const RefVar&, const RefVar&)
MPW-mangled: IsSubclass__FRC6RefVarT1
*/
    JT IsSubclass__FRC6RefVarT1, 018029b0

/*
C++: IsSymbol(const RefVar&)
MPW-mangled: IsSymbol__FRC6RefVar
*/
    JT _Z8IsSymbolRK6RefVar, 018029b4

/*
C++: IsSymbol(long)
MPW-mangled: IsSymbol__Fl
*/
    JT _Z8IsSymboll, 01800970

/*
C++: IsTab(unsigned short)
MPW-mangled: IsTab__FUs
*/
    JT _Z5IsTabt, 01801c14

/*
C++: IsVccOffNotifyRegistered(int)
MPW-mangled: IsVccOffNotifyRegistered__Fi
*/
    JT _Z24IsVccOffNotifyRegisteredi, 01802aa0

/*
C++: TCardSocket::IsVccOn()
MPW-mangled: IsVccOn__11TCardSocketFv
*/
    JT _ZN11TCardSocket7IsVccOnEv, 018011fc

/*
C++: TCardSocket::IsVppOn()
MPW-mangled: IsVppOn__11TCardSocketFv
*/
    JT _ZN11TCardSocket7IsVppOnEv, 01801200

/*
C++: IsWhiteSpace(unsigned short)
MPW-mangled: IsWhiteSpace__FUs
*/
    JT _Z12IsWhiteSpacet, 01801c18

/*
C++: TCardSocket::IsWriteProtected()
MPW-mangled: IsWriteProtected__11TCardSocketFv
*/
    JT _ZN11TCardSocket16IsWriteProtectedEv, 01801204

/*
C++: TView::JustifyBounds(TRect*)
MPW-mangled: JustifyBounds__5TViewFP5TRect
*/
    JT _ZN5TView13JustifyBoundsEP5TRect, 01800c50

/*
C++: TCommTool::KillGetComplete(long)
MPW-mangled: KillGetComplete__9TCommToolFl
*/
    JT _ZN9TCommTool15KillGetCompleteEl, 018006d8

/*
C++: TCommTool::KillPutComplete(long)
MPW-mangled: KillPutComplete__9TCommToolFl
*/
    JT _ZN9TCommTool15KillPutCompleteEl, 018006dc

/*
C++: TCommTool::KillRequestComplete(CommToolRequestType, long)
MPW-mangled: KillRequestComplete__9TCommToolF19CommToolRequestTypel
*/
    JT _ZN9TCommTool19KillRequestCompleteE19CommToolRequestTypel, 018006e0

/*
C++: LSearch(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: LSearch__FRC6RefVarN41
*/
    JT LSearch__FRC6RefVarN41, 01802bb8
    JT LargestFreeInHeap, 01802b24

/*
C++: TSoupIndex::Last(SKey*, SKey*)
MPW-mangled: Last__10TSoupIndexFP4SKeyT1
*/
    JT Last__10TSoupIndexFP4SKeyT1, 01800974

/*
C++: CBufferList::Last()
MPW-mangled: Last__11CBufferListFv
*/
    JT _ZN11CBufferList4LastEv, 01801c1c

/*
C++: Length(const RefVar&)
MPW-mangled: Length__FRC6RefVar
*/
    JT _Z6LengthRK6RefVar, 018029b8

/*
C++: Length(long)
MPW-mangled: Length__Fl
*/
    JT _Z6Lengthl, 01800978

/*
C++: Librarian()
MPW-mangled: Librarian__Fv
*/
    JT _Z9Librarianv, 01801e60

/*
C++: TEndpointClient::ListenComplete(TEndpointEvent*)
MPW-mangled: ListenComplete__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient14ListenCompleteEP14TEndpointEvent, 01800380

/*
C++: TCommTool::ListenComplete(long)
MPW-mangled: ListenComplete__9TCommToolFl
*/
    JT _ZN9TCommTool14ListenCompleteEl, 018006e8

/*
C++: TCommTool::ListenStart()
MPW-mangled: ListenStart__9TCommToolFv
*/
    JT _ZN9TCommTool11ListenStartEv, 018006ec

/*
C++: TCommTool::Listen()
MPW-mangled: Listen__9TCommToolFv
*/
    JT _ZN9TCommTool6ListenEv, 018006e4

/*
C++: TEndpoint::Listen(TOptionArray*, TOptionArray*, CBufferSegment*, long*, unsigned long)
MPW-mangled: Listen__9TEndpointFP12TOptionArrayT1P14CBufferSegmentPlUl
*/
    JT Listen__9TEndpointFP12TOptionArrayT1P14CBufferSegmentPlUl, 0180037c

/*
C++: TCardATALoader::LoadATAPackages(TCardSocket*, TCardPCMCIA*, TATABootParamBlock*, void*, unsigned long)
MPW-mangled: LoadATAPackages__14TCardATALoaderFP11TCardSocketP11TCardPCMCIAP18TATABootParamBlockPvUl
*/
    JT _ZN14TCardATALoader15LoadATAPackagesEP11TCardSocketP11TCardPCMCIAP18TATABootParamBlockPvm, 01802d94

/*
C++: TCardATALoader::LoadDriverPackage(TATA*, TCardSocket*, unsigned long, unsigned long, unsigned char**, unsigned long*)
MPW-mangled: LoadDriverPackage__14TCardATALoaderFP4TATAP11TCardSocketUlT3PPUcPUl
*/
    JT LoadDriverPackage__14TCardATALoaderFP4TATAP11TCardSocketUlT3PPUcPUl, 01802aa4

/*
C++: LoadPackage(TEndpointPipe*, unsigned long*, unsigned char)
MPW-mangled: LoadPackage__FP13TEndpointPipePUlUc
*/
    JT _Z11LoadPackageP13TEndpointPipePmh, 0180120c

/*
C++: LoadPackage(CPipe*, SourceType, unsigned long*)
MPW-mangled: LoadPackage__FP5CPipe10SourceTypePUl
*/
    JT _Z11LoadPackageP5CPipe10SourceTypePm, 01801210

/*
C++: LoadPackage(CPipe*, unsigned long*, unsigned char)
MPW-mangled: LoadPackage__FP5CPipePUlUc
*/
    JT _Z11LoadPackageP5CPipePmh, 01801214

/*
C++: LoadPackage(char*, SourceType, unsigned long*)
MPW-mangled: LoadPackage__FPc10SourceTypePUl
*/
    JT _Z11LoadPackagePc10SourceTypePm, 01801218

/*
C++: TView::LocalOrigin() const
MPW-mangled: LocalOrigin__5TViewCFv
*/
    JT LocalOrigin__5TViewCFv, 01800c54

/*
C++: TNSDebugAPI::Locals(long)
MPW-mangled: Locals__11TNSDebugAPIFl
*/
    JT _ZN11TNSDebugAPI6LocalsEl, 01802814
    JT LockHeapRange, 01801220
    JT LockPtr, 018017e8

/*
C++: LockRef(long)
MPW-mangled: LockRef__Fl
*/
    JT _Z7LockRefl, 01800980
    JT LockStack, 01801224

/*
C++: LockedBinaryPtr(const RefVar&)
MPW-mangled: LockedBinaryPtr__FRC6RefVar
*/
    JT _Z15LockedBinaryPtrRK6RefVar, 018029bc

/*
C++: TDate::LongDateString(unsigned long, unsigned short*, unsigned long)
MPW-mangled: LongDateString__5TDateFUlPUsT1
*/
    JT LongDateString__5TDateFUlPUsT1, 01802620

/*
C++: TLookupNames::LookupNames(unsigned short*, unsigned long, unsigned char, unsigned long, unsigned long, unsigned long)
MPW-mangled: LookupNames__12TLookupNamesFPUsUlUcN32
*/
    JT LookupNames__12TLookupNamesFPUsUlUcN32, 01802534

/*
C++: TLookupNames::LookupNames(unsigned short*, unsigned long, unsigned char, unsigned long, TLookupRetryInfo*)
MPW-mangled: LookupNames__12TLookupNamesFPUsUlUcT2P16TLookupRetryInfo
*/
    JT LookupNames__12TLookupNamesFPUsUlUcT2P16TLookupRetryInfo, 01802530

/*
C++: TUNameServer::Lookup(char*, char*, unsigned long*, unsigned long*)
MPW-mangled: Lookup__12TUNameServerFPcT1PUlT3
*/
    JT _ZN12TUNameServer6LookupEPcS0_PmS1_, 01801228
    JT _ZN12TUNameServer6LookupEPKcS1_PmS2_, 01801228   /* const char* variant */
/*
C++: LowercaseText(unsigned short*, long)
MPW-mangled: LowercaseText__FPUsl
*/
    JT _Z13LowercaseTextPtl, 01801c20

/*
C++: MAKEBOOLEAN(int)
MPW-mangled: MAKEBOOLEAN__Fi
*/
    JT _Z11MAKEBOOLEANi, 01800984

/*
C++: MAKECHAR(unsigned short)
MPW-mangled: MAKECHAR__FUs
*/
    JT _Z8MAKECHARt, 01800990

/*
C++: MAKECHAR(char)
MPW-mangled: MAKECHAR__Fc
*/
    JT _Z8MAKECHARc, 0180098c

/*
C++: MAKEIMMED(unsigned long, unsigned long)
MPW-mangled: MAKEIMMED__FUlT1
*/
    JT MAKEIMMED__FUlT1, 01800994

/*
C++: TUPhys::MakeAccessible()
MPW-mangled: MakeAccessible__6TUPhysFv
*/
    JT _ZN6TUPhys14MakeAccessibleEv, 0180253c

/*
C++: TClassInfo::MakeAt(const void*) const
MPW-mangled: MakeAt__10TClassInfoCFPCv
*/
    JT MakeAt__10TClassInfoCFPCv, 0180122c

/*
C++: MakeBoolean(int)
MPW-mangled: MakeBoolean__Fi
*/
    JT _Z11MakeBooleani, 018029c0

/*
C++: MakeCFunction(long (*)(const RefVar&, ...), long, char*)
MPW-mangled: MakeCFunction__FPFRC6RefVare_llPc
*/
    JT MakeCFunction__FPFRC6RefVare_llPc, 01800988

/*
C++: MakeChar(unsigned char)
MPW-mangled: MakeChar__FUc
*/
    JT _Z8MakeCharh, 018029c4

/*
C++: TUPhys::MakeInaccessible()
MPW-mangled: MakeInaccessible__6TUPhysFv
*/
    JT _ZN6TUPhys16MakeInaccessibleEv, 0180249c

/*
C++: MakeInt(long)
MPW-mangled: MakeInt__Fl
*/
    JT _Z7MakeIntl, 018029c8

/*
C++: TWRecognizer::MakeNewGroupFromStroke(TStrokeUnit*)
MPW-mangled: MakeNewGroupFromStroke__12TWRecognizerFP11TStrokeUnit
*/
    JT _ZN12TWRecognizer22MakeNewGroupFromStrokeEP11TStrokeUnit, 01802d44

/*
C++: TUSoundChannel::MakeNode(SoundNode**)
MPW-mangled: MakeNode__14TUSoundChannelFPP9SoundNode
*/
    JT _ZN14TUSoundChannel8MakeNodeEPP9SoundNode, 01802c3c

/*
C++: TUObject::MakeObject(ObjectTypes, ObjectMessage*, unsigned long)
MPW-mangled: MakeObject__8TUObjectF11ObjectTypesP13ObjectMessageUl
*/
    JT _ZN8TUObject10MakeObjectE11ObjectTypesP13ObjectMessagem, 01801230

/*
C++: MakePeripheralController(PeripheralType)
MPW-mangled: MakePeripheralController__F14PeripheralType
*/
    JT _Z24MakePeripheralController14PeripheralType, 01801f60

/*
C++: MakePhysInaccessible(unsigned long)
MPW-mangled: MakePhysInaccessible__FUl
*/
    JT _Z20MakePhysInaccessiblem, 018024a0

/*
C++: MakeReal(double)
MPW-mangled: MakeReal__Fd
*/
    JT _Z8MakeReald, 01800998

/*
C++: CMemObject::MakeShared(unsigned long)
MPW-mangled: MakeShared__10CMemObjectFUl
*/
    JT _ZN10CMemObject10MakeSharedEm, 01801c30

/*
C++: CRingBuffer::MakeShared(unsigned long)
MPW-mangled: MakeShared__11CRingBufferFUl
*/
    JT _ZN11CRingBuffer10MakeSharedEm, 01801c34

/*
C++: TOptionArray::MakeShared(unsigned long)
MPW-mangled: MakeShared__12TOptionArrayFUl
*/
    JT _ZN12TOptionArray10MakeSharedEm, 01800384

/*
C++: CBufferSegment::MakeShared(unsigned long)
MPW-mangled: MakeShared__14CBufferSegmentFUl
*/
    JT _ZN14CBufferSegment10MakeSharedEm, 01801c38

/*
C++: MakeSimplePattern(char*)
MPW-mangled: MakeSimplePattern__FPc
*/
    JT _Z17MakeSimplePatternPc, 01800c5c

/*
C++: MakeString(const unsigned short*)
MPW-mangled: MakeString__FPCUs
*/
    JT _Z10MakeStringPKt, 018009a0

/*
C++: MakeString(const char*)
MPW-mangled: MakeString__FPCc
*/
    JT _Z10MakeStringPKc, 0180099c

/*
C++: MakeSymbol(char*)
MPW-mangled: MakeSymbol__FPc
*/
    JT _Z10MakeSymbolPc, 018029cc

/*
C++: CMemObject::Make(unsigned long, TUMsgToken*)
MPW-mangled: Make__10CMemObjectFUlP10TUMsgToken
*/
    JT _ZN10CMemObject4MakeEmP10TUMsgToken, 01801c24

/*
C++: static CList::Make(long)
MPW-mangled: Make__5CListSFl
*/
    JT _ZN5CList4MakeEl, 01801c28

/*
C++: static CList::Make()
MPW-mangled: Make__5CListSFv
*/
    JT _ZN5CList4MakeEv, 01801c2c

/*
C++: MapSlots(const RefVar&, long (*)(const RefVar&, const RefVar&, unsigned long), unsigned long)
MPW-mangled: MapSlots__FRC6RefVarPFRC6RefVarT1Ul_lUl
*/
    JT MapSlots__FRC6RefVarPFRC6RefVarT1Ul_lUl, 018009a4

/*
C++: TLicenseeVAddress::Map(unsigned char, Perm)
MPW-mangled: Map__17TLicenseeVAddressFUc4Perm
*/
    JT _ZN17TLicenseeVAddress3MapEh4Perm, 01802aa8

/*
C++: TCircleBuf::MarkerCount()
MPW-mangled: MarkerCount__10TCircleBufFv
*/
    JT _ZN10TCircleBuf11MarkerCountEv, 01802a3c

/*
C++: TCircleBuf::MarkerSpace()
MPW-mangled: MarkerSpace__10TCircleBufFv
*/
    JT _ZN10TCircleBuf11MarkerSpaceEv, 01802a40
    JT MaskInterrupt, 01802bdc

/*
C++: MatchString(AirusAParmBlock**, char*, const RefVar&)
MPW-mangled: MatchString__FPP15AirusAParmBlockPcRC6RefVar
*/
    JT _Z11MatchStringPP15AirusAParmBlockPcRK6RefVar, 01802778
    JT MaxHeapSize, 0180168c
    JT MemError, 018017ec

/*
C++: TOptionArray::Merge(TOptionArray*)
MPW-mangled: Merge__12TOptionArrayFP12TOptionArray
*/
    JT _ZN12TOptionArray5MergeEP12TOptionArray, 01800388

/*
C++: CDynamicArray::Merge(CDynamicArray*)
MPW-mangled: Merge__13CDynamicArrayFP13CDynamicArray
*/
    JT _ZN13CDynamicArray5MergeEP13CDynamicArray, 01801c3c

/*
C++: TCardMessage::MessageStuff(unsigned long, unsigned long, unsigned long)
MPW-mangled: MessageStuff__12TCardMessageFUlN21
*/
    JT MessageStuff__12TCardMessageFUlN21, 01801238

/*
C++: MinMax_noninline(long, long, long)
MPW-mangled: MinMax_noninline__FlN21
*/
    JT MinMax_noninline__FlN21, 01801c40

/*
C++: TPackageIterator::ModifyDate()
MPW-mangled: ModifyDate__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator10ModifyDateEv, 0180123c

/*
C++: TPMIterator::More()
MPW-mangled: More__11TPMIteratorFv
*/
    JT _ZN11TPMIterator4MoreEv, 01801240

/*
C++: CArrayIterator::More()
MPW-mangled: More__14CArrayIteratorFv
*/
    JT _ZN14CArrayIterator4MoreEv, 01801c44

/*
C++: TOptionIterator::More()
MPW-mangled: More__15TOptionIteratorFv
*/
    JT _ZN15TOptionIterator4MoreEv, 0180038c
    JT MoveBytes, 01801690
    JT MoveHHi, 018017f0

/*
C++: TView::Move(const TPoint&)
MPW-mangled: Move__5TViewFRC6TPoint
*/
    JT _ZN5TView4MoveERK6TPoint, 01800c60

/*
C++: NSCallGlobalFnWithArgArray(const RefVar&, const RefVar&)
MPW-mangled: NSCallGlobalFnWithArgArray__FRC6RefVarT1
*/
    JT NSCallGlobalFnWithArgArray__FRC6RefVarT1, 01802704

/*
C++: NSCallGlobalFn(const RefVar&)
MPW-mangled: NSCallGlobalFn__FRC6RefVar
*/
    JT _Z14NSCallGlobalFnRK6RefVar, 018026e8

/*
C++: NSCallGlobalFn(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSCallGlobalFn__FRC6RefVarN21
*/
    JT _Z14NSCallGlobalFnRK6RefVarS1_S1_, 018026f0

/*
C++: NSCallGlobalFn(const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSCallGlobalFn__FRC6RefVarN31
*/
    JT _Z14NSCallGlobalFnRK6RefVarS1_S1_S1_, 018026f4

/*
C++: NSCallGlobalFn(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSCallGlobalFn__FRC6RefVarN41
*/
    JT _Z14NSCallGlobalFnRK6RefVarS1_S1_S1_S1_, 018026f8

/*
C++: NSCallGlobalFn(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSCallGlobalFn__FRC6RefVarN51
*/
    JT _Z14NSCallGlobalFnRK6RefVarS1_S1_S1_S1_S1_, 018026fc

/*
C++: NSCallGlobalFn(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSCallGlobalFn__FRC6RefVarN61
*/
    JT _Z14NSCallGlobalFnRK6RefVarS1_S1_S1_S1_S1_S1_, 01802700

/*
C++: NSCallGlobalFn(const RefVar&, const RefVar&)
MPW-mangled: NSCallGlobalFn__FRC6RefVarT1
*/
    JT _Z14NSCallGlobalFnRK6RefVarS1_, 018026ec

/*
C++: NSCallWithArgArray(const RefVar&, const RefVar&)
MPW-mangled: NSCallWithArgArray__FRC6RefVarT1
*/
    JT NSCallWithArgArray__FRC6RefVarT1, 018029d0

/*
C++: NSCall(const RefVar&)
MPW-mangled: NSCall__FRC6RefVar
*/
    JT _Z6NSCallRK6RefVar, 018026cc

/*
C++: NSCall(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSCall__FRC6RefVarN21
*/
    JT _Z6NSCallRK6RefVarS1_S1_, 018026d4

/*
C++: NSCall(const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSCall__FRC6RefVarN31
*/
    JT _Z6NSCallRK6RefVarS1_S1_S1_, 018026d8

/*
C++: NSCall(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSCall__FRC6RefVarN41
*/
    JT _Z6NSCallRK6RefVarS1_S1_S1_S1_, 018026dc

/*
C++: NSCall(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSCall__FRC6RefVarN51
*/
    JT _Z6NSCallRK6RefVarS1_S1_S1_S1_S1_, 018026e0

/*
C++: NSCall(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSCall__FRC6RefVarN61
*/
    JT _Z6NSCallRK6RefVarS1_S1_S1_S1_S1_S1_, 018026e4

/*
C++: NSCall(const RefVar&, const RefVar&)
MPW-mangled: NSCall__FRC6RefVarT1
*/
    JT _Z6NSCallRK6RefVarS1_, 018026d0
/*
C++: NSGetGlobalFn(const RefVar&)
MPW-mangled: NSGetGlobalFn__FRC6RefVar
*/
    JT _Z13NSGetGlobalFnRK6RefVar, 01802708

/*
C++: NSSendIfDefinedWithArgArray(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendIfDefinedWithArgArray__FRC6RefVarN21
*/
    JT NSSendIfDefinedWithArgArray__FRC6RefVarN21, 01802880

/*
C++: NSSendIfDefined(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendIfDefined__FRC6RefVarN21
*/
    JT NSSendIfDefined__FRC6RefVarN21, 01802868

/*
C++: NSSendIfDefined(const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendIfDefined__FRC6RefVarN31
*/
    JT NSSendIfDefined__FRC6RefVarN31, 0180286c

/*
C++: NSSendIfDefined(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendIfDefined__FRC6RefVarN41
*/
    JT NSSendIfDefined__FRC6RefVarN41, 01802870

/*
C++: NSSendIfDefined(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendIfDefined__FRC6RefVarN51
*/
    JT NSSendIfDefined__FRC6RefVarN51, 01802874

/*
C++: NSSendIfDefined(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendIfDefined__FRC6RefVarN61
*/
    JT NSSendIfDefined__FRC6RefVarN61, 01802878

/*
C++: NSSendIfDefined(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendIfDefined__FRC6RefVarN71
*/
    JT NSSendIfDefined__FRC6RefVarN71, 0180287c

/*
C++: NSSendIfDefined(const RefVar&, const RefVar&)
MPW-mangled: NSSendIfDefined__FRC6RefVarT1
*/
    JT NSSendIfDefined__FRC6RefVarT1, 01802864

/*
C++: NSSendProtoIfDefinedWithArgArray(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProtoIfDefinedWithArgArray__FRC6RefVarN21
*/
    JT NSSendProtoIfDefinedWithArgArray__FRC6RefVarN21, 018028a0

/*
C++: NSSendProtoIfDefined(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProtoIfDefined__FRC6RefVarN21
*/
    JT NSSendProtoIfDefined__FRC6RefVarN21, 01802888

/*
C++: NSSendProtoIfDefined(const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProtoIfDefined__FRC6RefVarN31
*/
    JT NSSendProtoIfDefined__FRC6RefVarN31, 0180288c

/*
C++: NSSendProtoIfDefined(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProtoIfDefined__FRC6RefVarN41
*/
    JT NSSendProtoIfDefined__FRC6RefVarN41, 01802890

/*
C++: NSSendProtoIfDefined(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProtoIfDefined__FRC6RefVarN51
*/
    JT NSSendProtoIfDefined__FRC6RefVarN51, 01802894

/*
C++: NSSendProtoIfDefined(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProtoIfDefined__FRC6RefVarN61
*/
    JT NSSendProtoIfDefined__FRC6RefVarN61, 01802898

/*
C++: NSSendProtoIfDefined(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProtoIfDefined__FRC6RefVarN71
*/
    JT NSSendProtoIfDefined__FRC6RefVarN71, 0180289c

/*
C++: NSSendProtoIfDefined(const RefVar&, const RefVar&)
MPW-mangled: NSSendProtoIfDefined__FRC6RefVarT1
*/
    JT NSSendProtoIfDefined__FRC6RefVarT1, 01802884

/*
C++: NSSendProtoWithArgArray(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProtoWithArgArray__FRC6RefVarN21
*/
    JT NSSendProtoWithArgArray__FRC6RefVarN21, 01802860

/*
C++: NSSendProto(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProto__FRC6RefVarN21
*/
    JT NSSendProto__FRC6RefVarN21, 01802848

/*
C++: NSSendProto(const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProto__FRC6RefVarN31
*/
    JT NSSendProto__FRC6RefVarN31, 0180284c

/*
C++: NSSendProto(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProto__FRC6RefVarN41
*/
    JT NSSendProto__FRC6RefVarN41, 01802850

/*
C++: NSSendProto(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProto__FRC6RefVarN51
*/
    JT NSSendProto__FRC6RefVarN51, 01802854

/*
C++: NSSendProto(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProto__FRC6RefVarN61
*/
    JT NSSendProto__FRC6RefVarN61, 01802858

/*
C++: NSSendProto(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendProto__FRC6RefVarN71
*/
    JT NSSendProto__FRC6RefVarN71, 0180285c

/*
C++: NSSendProto(const RefVar&, const RefVar&)
MPW-mangled: NSSendProto__FRC6RefVarT1
*/
    JT NSSendProto__FRC6RefVarT1, 01802844

/*
C++: NSSendRootMessageWithArgArray(const RefVar&, const RefVar&)
MPW-mangled: NSSendRootMessageWithArgArray__FRC6RefVarT1
*/
    JT NSSendRootMessageWithArgArray__FRC6RefVarT1, 01802bf4

/*
C++: NSSendRootMessage(const RefVar&)
MPW-mangled: NSSendRootMessage__FRC6RefVar
*/
    JT _Z17NSSendRootMessageRK6RefVar, 01802bf8

/*
C++: NSSendRootMessage(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendRootMessage__FRC6RefVarN21
*/
    JT NSSendRootMessage__FRC6RefVarN21, 01802bfc

/*
C++: NSSendRootMessage(const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendRootMessage__FRC6RefVarN31
*/
    JT NSSendRootMessage__FRC6RefVarN31, 01802c00

/*
C++: NSSendRootMessage(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendRootMessage__FRC6RefVarN41
*/
    JT NSSendRootMessage__FRC6RefVarN41, 01802c04

/*
C++: NSSendRootMessage(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendRootMessage__FRC6RefVarN51
*/
    JT NSSendRootMessage__FRC6RefVarN51, 01802c08

/*
C++: NSSendRootMessage(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendRootMessage__FRC6RefVarN61
*/
    JT NSSendRootMessage__FRC6RefVarN61, 01802c0c

/*
C++: NSSendRootMessage(const RefVar&, const RefVar&)
MPW-mangled: NSSendRootMessage__FRC6RefVarT1
*/
    JT NSSendRootMessage__FRC6RefVarT1, 01802c10

/*
C++: NSSendWithArgArray(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSendWithArgArray__FRC6RefVarN21
*/
    JT NSSendWithArgArray__FRC6RefVarN21, 01802840

/*
C++: NSSend(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSend__FRC6RefVarN21
*/
    JT _Z6NSSendRK6RefVarS1_S1_, 01802710

/*
C++: NSSend(const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSend__FRC6RefVarN31
*/
    JT NSSend__FRC6RefVarN31, 01802714

/*
C++: NSSend(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSend__FRC6RefVarN41
*/
    JT NSSend__FRC6RefVarN41, 01802718

/*
C++: NSSend(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSend__FRC6RefVarN51
*/
    JT NSSend__FRC6RefVarN51, 0180271c

/*
C++: NSSend(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSend__FRC6RefVarN61
*/
    JT NSSend__FRC6RefVarN61, 01802720

/*
C++: NSSend(const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: NSSend__FRC6RefVarN71
*/
    JT NSSend__FRC6RefVarN71, 01802724

/*
C++: NSSend(const RefVar&, const RefVar&)
MPW-mangled: NSSend__FRC6RefVarT1
*/
    JT _Z6NSSendRK6RefVarS1_, 0180270c

/*
C++: NativeEntry(const RefVar&, long, RefHandle**)
MPW-mangled: NativeEntry__FRC6RefVarlPP9RefHandle
*/
    JT _Z11NativeEntryRK6RefVarlPP9RefHandle, 01802750

/*
C++: TAppWorldState::NestedEventLoop()
MPW-mangled: NestedEventLoop__14TAppWorldStateFv
*/
    JT _ZN14TAppWorldState15NestedEventLoopEv, 01801ef8

/*
C++: TAppWorld::NestedEventLoop()
MPW-mangled: NestedEventLoop__9TAppWorldFv
*/
    JT _ZN9TAppWorld15NestedEventLoopEv, 01801c48

/*
C++: NewByName(const char*, const char*, const char*)
MPW-mangled: NewByName__FPCcN21
*/
    JT NewByName__FPCcN21, 01801254

/*
C++: NewByName(const char*, const char*)
MPW-mangled: NewByName__FPCcT1
*/
    JT _Z9NewByNamePKcS0_, 01801258

/*
C++: NewByName(const char*, const char*, unsigned long)
MPW-mangled: NewByName__FPCcT1Ul
*/
    JT NewByName__FPCcT1Ul, 0180125c

/*
C++: TSystemEventHandler::NewCard(TAEvent*)
MPW-mangled: NewCard__19TSystemEventHandlerFP7TAEvent
*/
    JT _ZN19TSystemEventHandler7NewCardEP7TAEvent, 01801c4c

/*
C++: TWRecognizer::NewClassification(TWRecUnit*)
MPW-mangled: NewClassification__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer17NewClassificationEP9TWRecUnit, 01802d48
    JT NewFakeHandle, 018017f4

/*
C++: NewFromHunkByName(void*, const char*, const char*)
MPW-mangled: NewFromHunkByName__FPvPCcT2
*/
    JT NewFromHunkByName__FPvPCcT2, 01801260
    JT NewHandle, 018017f8
    JT NewHandleClear, 018017fc
    JT NewHeap, 01801694
    JT NewHeapArea, 01801264
    JT NewHeapAt, 01801800
    JT NewHeapDomain, 01801268

/*
C++: NewIterator(const RefVar&)
MPW-mangled: NewIterator__FRC6RefVar
*/
    JT _Z11NewIteratorRK6RefVar, 018009a8

/*
C++: NewNSDebugAPI(TInterpreter*)
MPW-mangled: NewNSDebugAPI__FP12TInterpreter
*/
    JT _Z13NewNSDebugAPIP12TInterpreter, 01802838

/*
C++: static TRealTimeClock::NewName(unsigned long*)
MPW-mangled: NewName__14TRealTimeClockSFPUl
*/
    JT _ZN14TRealTimeClock7NewNameEPm, 01802c14

/*
C++: static TURealTimeAlarm::NewName(unsigned long*)
MPW-mangled: NewName__15TURealTimeAlarmSFPUl
*/
    JT _ZN15TURealTimeAlarm7NewNameEPm, 01802c18
    JT NewNamedHandle, 01801804
    JT NewNamedPtr, 01801808

/*
C++: NewPackage(CPipe*, const RefVar&, const RefVar&, unsigned long)
MPW-mangled: NewPackage__FP5CPipeRC6RefVarT2Ul
*/
    JT NewPackage__FP5CPipeRC6RefVarT2Ul, 01802770
    JT NewPersistentVMHeap, 018027b4
    JT NewPtr, 0180180c
    JT NewPtrClear, 01801810
    JT NewSegregatedVMHeap, 01801814
    JT NewStack, 01801270

/*
C++: NewTObjectIterator(const RefVar&)
MPW-mangled: NewTObjectIterator__FRC6RefVar
*/
    JT _Z18NewTObjectIteratorRK6RefVar, 018029d4
    JT NewVMHeap, 01801818
    JT NewWiredPtr, 0180181c

/*
C++: TClassInfo::New() const
MPW-mangled: New__10TClassInfoCFv
*/
    JT _ZNK10TClassInfo3NewEv, 01801248

/*
C++: static TCardHandler::New(char*)
MPW-mangled: New__12TCardHandlerSFPc
*/
    JT _ZN12TCardHandler3NewEPc, 0180124c

/*
C++: static TClassInfoRegistry::New(const char*)
MPW-mangled: New__18TClassInfoRegistrySFPCc
*/
    JT _ZN18TClassInfoRegistry3NewEPKc, 01801250

/*
C++: static TATA::New(char*)
MPW-mangled: New__4TATASFPc
*/
    JT _ZN4TATA3NewEPc, 01802aac

/*
C++: static TEndpoint::New(char*)
MPW-mangled: New__9TEndpointSFPc
*/
    JT _ZN9TEndpoint3NewEPc, 01800390

/*
C++: NextDateOfMonth(TDate*, unsigned long)
MPW-mangled: NextDateOfMonth__FP5TDateUl
*/
    JT _Z15NextDateOfMonthP5TDatem, 01802624

/*
C++: NextDateOfYear(TDate*, unsigned long)
MPW-mangled: NextDateOfYear__FP5TDateUl
*/
    JT _Z14NextDateOfYearP5TDatem, 01802628

/*
C++: NextDayOfWeek(TDate*, unsigned long)
MPW-mangled: NextDayOfWeek__FP5TDateUl
*/
    JT _Z13NextDayOfWeekP5TDatem, 0180262c
    JT NextHandler, 01801820
    JT NextHeapBlock, 01801698

/*
C++: CArrayIterator::NextIndex()
MPW-mangled: NextIndex__14CArrayIteratorFv
*/
    JT _ZN14CArrayIterator9NextIndexEv, 01801c68

/*
C++: TOptionIterator::NextIndex()
MPW-mangled: NextIndex__15TOptionIteratorFv
*/
    JT _ZN15TOptionIterator9NextIndexEv, 01800394

/*
C++: CListIterator::NextItem()
MPW-mangled: NextItem__13CListIteratorFv
*/
    JT _ZN13CListIterator8NextItemEv, 01801c6c

/*
C++: NextMeeting(TDate*, unsigned long, unsigned long, unsigned long)
MPW-mangled: NextMeeting__FP5TDateUlN22
*/
    JT NextMeeting__FP5TDateUlN22, 01802630

/*
C++: TOptionIterator::NextOption()
MPW-mangled: NextOption__15TOptionIteratorFv
*/
    JT _ZN15TOptionIterator10NextOptionEv, 01800398

/*
C++: TPMIterator::NextPackage()
MPW-mangled: NextPackage__11TPMIteratorFv
*/
    JT _ZN11TPMIterator11NextPackageEv, 01801278

/*
C++: NextPeriod(TDate*, unsigned long, unsigned long)
MPW-mangled: NextPeriod__FP5TDateUlT2
*/
    JT NextPeriod__FP5TDateUlT2, 01802634

/*
C++: TSoupIndex::Next(SKey*, SKey*, int, SKey*, SKey*)
MPW-mangled: Next__10TSoupIndexFP4SKeyT1iN21
*/
    JT Next__10TSoupIndexFP4SKeyT1iN21, 018009b0

/*
C++: CBufferList::Next()
MPW-mangled: Next__11CBufferListFv
*/
    JT _ZN11CBufferList4NextEv, 01801c50

/*
C++: CBufferPipe::Next()
MPW-mangled: Next__11CBufferPipeFv
*/
    JT _ZN11CBufferPipe4NextEv, 01801c54

/*
C++: CRingBuffer::Next()
MPW-mangled: Next__11CRingBufferFv
*/
    JT _ZN11CRingBuffer4NextEv, 01801c58

/*
C++: CBufferSegment::Next()
MPW-mangled: Next__14CBufferSegmentFv
*/
    JT _ZN14CBufferSegment4NextEv, 01801c5c

/*
C++: TObjectIterator::Next()
MPW-mangled: Next__15TObjectIteratorFv
*/
    JT _ZN15TObjectIterator4NextEv, 018009b4

/*
C++: CShadowRingBuffer::Next()
MPW-mangled: Next__17CShadowRingBufferFv
*/
    JT _ZN17CShadowRingBuffer4NextEv, 01801c60

/*
C++: TClassInfoRegistry::Next(long, const TClassInfo*, unsigned long*) const
MPW-mangled: Next__18TClassInfoRegistryCFlPC10TClassInfoPUl
*/
    JT Next__18TClassInfoRegistryCFlPC10TClassInfoPUl, 01801274

/*
C++: CShadowBufferSegment::Next()
MPW-mangled: Next__20CShadowBufferSegmentFv
*/
    JT _ZN20CShadowBufferSegment4NextEv, 01801c64

/*
C++: TStoreHashTableIterator::Next()
MPW-mangled: Next__23TStoreHashTableIteratorFv
*/
    JT _ZN23TStoreHashTableIterator4NextEv, 018009b8

/*
C++: NoDiacriticsText(unsigned short*, long)
MPW-mangled: NoDiacriticsText__FPUsl
*/
    JT _Z16NoDiacriticsTextPtl, 01801c70

/*
C++: TNSDebugAPI::NumStackFrames()
MPW-mangled: NumStackFrames__11TNSDebugAPIFv
*/
    JT _ZN11TNSDebugAPI14NumStackFramesEv, 018027f0

/*
C++: TNSDebugAPI::NumTemps(long)
MPW-mangled: NumTemps__11TNSDebugAPIFl
*/
    JT _ZN11TNSDebugAPI8NumTempsEl, 01802828

/*
C++: TPackageIterator::NumberOfParts()
MPW-mangled: NumberOfParts__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator13NumberOfPartsEv, 0180127c

/*
C++: NumberString(double, unsigned short*, unsigned long, char*)
MPW-mangled: NumberString__FdPUsUlPc
*/
    JT _Z12NumberStringdPtmPc, 01801c74

/*
C++: OSErrorAlert(char*)
MPW-mangled: OSErrorAlert__FPc
*/
    JT _Z12OSErrorAlertPc, 01801824

/*
C++: OSWarningAlert(char*)
MPW-mangled: OSWarningAlert__FPc
*/
    JT _Z14OSWarningAlertPc, 01801828

/*
C++: ObjectFlags(long)
MPW-mangled: ObjectFlags__Fl
*/
    JT _Z11ObjectFlagsl, 018009bc

/*
C++: ObjectPtr(long)
MPW-mangled: ObjectPtr__Fl
*/
    JT _Z9ObjectPtrl, 018009c0

/*
C++: TView::OffsetToCaret(long, TRect*)
MPW-mangled: OffsetToCaret__5TViewFlP5TRect
*/
    JT _ZN5TView13OffsetToCaretElP5TRect, 01800c68

/*
C++: TView::Offset(TPoint)
MPW-mangled: Offset__5TViewF6TPoint
*/
    JT _ZN5TView6OffsetE6TPoint, 01800c64

/*
C++: OpenAppleTalkPart(unsigned long, TUPort*)
MPW-mangled: OpenAppleTalkPart__FUlP6TUPort
*/
    JT _Z17OpenAppleTalkPartmP6TUPort, 0180208c

/*
C++: OpenAppleTalk(unsigned long)
MPW-mangled: OpenAppleTalk__FUl
*/
    JT _Z13OpenAppleTalkm, 0180206c

/*
C++: OpenCommTool(unsigned long, TOptionArray*, TCMService*)
MPW-mangled: OpenCommTool__FUlP12TOptionArrayP10TCMService
*/
    JT _Z12OpenCommToolmP12TOptionArrayP10TCMService, 018006f4

/*
C++: TCommTool::OpenComplete()
MPW-mangled: OpenComplete__9TCommToolFv
*/
    JT _ZN9TCommTool12OpenCompleteEv, 018006f8

/*
C++: TCommTool::OpenContinue()
MPW-mangled: OpenContinue__9TCommToolFv
*/
    JT _ZN9TCommTool12OpenContinueEv, 018006fc

/*
C++: OpenIREndpointForBitBang()
MPW-mangled: OpenIREndpointForBitBang__Fv
*/
    JT _Z24OpenIREndpointForBitBangv, 01801ef4

/*
C++: OpenPort(GrafPort*)
MPW-mangled: OpenPort__FP8GrafPort
*/
    JT _Z8OpenPortP8GrafPort, 01801e74

/*
C++: TATPInterface::OpenResponder(unsigned char*, unsigned short, unsigned char, unsigned char, unsigned long, unsigned char)
MPW-mangled: OpenResponder__13TATPInterfaceFPUcUsUcT3UlT3
*/
    JT OpenResponder__13TATPInterfaceFPUcUsUcT3UlT3, 01802090

/*
C++: TCommTool::OpenStart(TOptionArray*)
MPW-mangled: OpenStart__9TCommToolFP12TOptionArray
*/
    JT _ZN9TCommTool9OpenStartEP12TOptionArray, 01800700

/*
C++: TUSoundChannel::Open(int, int)
MPW-mangled: Open__14TUSoundChannelFiT1
*/
    JT Open__14TUSoundChannelFiT1, 01802c40

/*
C++: TCommTool::Open()
MPW-mangled: Open__9TCommToolFv
*/
    JT _ZN9TCommTool4OpenEv, 018006f0

/*
C++: TEndpoint::Open(unsigned long)
MPW-mangled: Open__9TEndpointFUl
*/
    JT _ZN9TEndpoint4OpenEm, 0180039c

/*
C++: TEndpointClient::OptMgmtComplete(TEndpointEvent*)
MPW-mangled: OptMgmtComplete__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient15OptMgmtCompleteEP14TEndpointEvent, 018003a8

/*
C++: TEndpoint::OptMgmt(unsigned long, TOptionArray*, unsigned long)
MPW-mangled: OptMgmt__9TEndpointFUlP12TOptionArrayT1
*/
    JT OptMgmt__9TEndpointFUlP12TOptionArrayT1, 018003a4

/*
C++: TOptionArray::OptionAt(long)
MPW-mangled: OptionAt__12TOptionArrayFl
*/
    JT _ZN12TOptionArray8OptionAtEl, 018003a0

/*
C++: TCommTool::OptionMgmt(TCommToolOptionMgmtRequest*)
MPW-mangled: OptionMgmt__9TCommToolFP26TCommToolOptionMgmtRequest
*/
    JT _ZN9TCommTool10OptionMgmtEP26TCommToolOptionMgmtRequest, 01800704

/*
C++: TView::OuterBounds(TRect*)
MPW-mangled: OuterBounds__5TViewFP5TRect
*/
    JT _ZN5TView11OuterBoundsEP5TRect, 01800c6c

/*
C++: TEndpointPipe::Overflow()
MPW-mangled: Overflow__13TEndpointPipeFv
*/
    JT _ZN13TEndpointPipe8OverflowEv, 018003ac

/*
C++: CNullPipe::Overflow()
MPW-mangled: Overflow__9CNullPipeFv
*/
    JT _ZN9CNullPipe8OverflowEv, 01801c78

/*
C++: CPartPipe::Overflow()
MPW-mangled: Overflow__9CPartPipeFv
*/
    JT _ZN9CPartPipe8OverflowEv, 01801280

/*
C++: CTaskPipe::Overflow()
MPW-mangled: Overflow__9CTaskPipeFv
*/
    JT _ZN9CTaskPipe8OverflowEv, 01801284

/*
C++: PCMCIAPowerInUse()
MPW-mangled: PCMCIAPowerInUse__Fv
*/
    JT _Z16PCMCIAPowerInUsev, 018012b4

/*
C++: TCardSocket::PCMCIAReset()
MPW-mangled: PCMCIAReset__11TCardSocketFv
*/
    JT _ZN11TCardSocket11PCMCIAResetEv, 018012b8

/*
C++: TNSDebugAPI::PC(long)
MPW-mangled: PC__11TNSDebugAPIFl
*/
    JT _ZN11TNSDebugAPI2PCEl, 018027fc

/*
C++: PackageAllocationOk(TStore*, unsigned long)
MPW-mangled: PackageAllocationOk__FP6TStoreUl
*/
    JT _Z19PackageAllocationOkP6TStorem, 01801288

/*
C++: PackageAvailable(TStore*, unsigned long, unsigned long*)
MPW-mangled: PackageAvailable__FP6TStoreUlPUl
*/
    JT _Z16PackageAvailableP6TStoremPm, 0180128c

/*
C++: TPackageIterator::PackageFlags()
MPW-mangled: PackageFlags__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator12PackageFlagsEv, 01801290

/*
C++: TPMIterator::PackageId()
MPW-mangled: PackageId__11TPMIteratorFv
*/
    JT _ZN11TPMIterator9PackageIdEv, 01801294

/*
C++: PackageManagerPortId()
MPW-mangled: PackageManagerPortId__Fv
*/
    JT _Z20PackageManagerPortIdv, 01801298

/*
C++: TPMIterator::PackageName()
MPW-mangled: PackageName__11TPMIteratorFv
*/
    JT _ZN11TPMIterator11PackageNameEv, 0180129c

/*
C++: TPackageIterator::PackageName()
MPW-mangled: PackageName__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator11PackageNameEv, 018012a0

/*
C++: TPMIterator::PackageSize()
MPW-mangled: PackageSize__11TPMIteratorFv
*/
    JT _ZN11TPMIterator11PackageSizeEv, 018012a4

/*
C++: TPackageIterator::PackageSize()
MPW-mangled: PackageSize__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator11PackageSizeEv, 018012a8

/*
C++: PackageUnavailable(unsigned long)
MPW-mangled: PackageUnavailable__FUl
*/
    JT _Z18PackageUnavailablem, 018012ac

/*
C++: TUDomainManager::PageMonProc(long, void*)
MPW-mangled: PageMonProc__15TUDomainManagerFlPv
*/
    JT _ZN15TUDomainManager11PageMonProcElPv, 018012b0

/*
C++: ParamString(unsigned short*, const long, const unsigned short*, ...)
MPW-mangled: ParamString__FPUsClPCUse
*/
    JT _Z11ParamStringPtKlPKtz, 01801c7c

/*
C++: ParseFile(char*)
MPW-mangled: ParseFile__FPc
*/
    JT _Z9ParseFilePc, 018009c4

/*
C++: TPCMCIA20Parser::ParsePCCardCIS(TCardPCMCIA*, TCardSocket*)
MPW-mangled: ParsePCCardCIS__15TPCMCIA20ParserFP11TCardPCMCIAP11TCardSocket
*/
    JT _ZN15TPCMCIA20Parser14ParsePCCardCISEP11TCardPCMCIAP11TCardSocket, 0180258c

/*
C++: TPCMCIA20Parser::ParsePCCardCIS(unsigned char*, unsigned char*, TCardPCMCIA*, TCardSocket*)
MPW-mangled: ParsePCCardCIS__15TPCMCIA20ParserFPUcT1P11TCardPCMCIAP11TCardSocket
*/
    JT ParsePCCardCIS__15TPCMCIA20ParserFPUcT1P11TCardPCMCIAP11TCardSocket, 01802590

/*
C++: ParseString(const RefVar&)
MPW-mangled: ParseString__FRC6RefVar
*/
    JT _Z11ParseStringRK6RefVar, 018009c8

/*
C++: TUSoundChannel::Pause(SoundBlock*, long*)
MPW-mangled: Pause__14TUSoundChannelFP10SoundBlockPl
*/
    JT _ZN14TUSoundChannel5PauseEP10SoundBlockPl, 01802c44

/*
C++: TInterpreter::PeekControl(long)
MPW-mangled: PeekControl__12TInterpreterFl
*/
    JT _ZN12TInterpreter11PeekControlEl, 01802004

/*
C++: TCircleBuf::PeekFirstLong(unsigned long*)
MPW-mangled: PeekFirstLong__10TCircleBufFPUl
*/
    JT _ZN10TCircleBuf13PeekFirstLongEPm, 01802a44

/*
C++: TCircleBuf::PeekNextByte(unsigned char*)
MPW-mangled: PeekNextByte__10TCircleBufFPUc
*/
    JT _ZN10TCircleBuf12PeekNextByteEPh, 01802a48

/*
C++: TCircleBuf::PeekNextByte(unsigned char*, unsigned long*)
MPW-mangled: PeekNextByte__10TCircleBufFPUcPUl
*/
    JT _ZN10TCircleBuf12PeekNextByteEPhPm, 01802df8

/*
C++: TInterpreter::PeekValue(long)
MPW-mangled: PeekValue__12TInterpreterFl
*/
    JT _ZN12TInterpreter9PeekValueEl, 01801ff8

/*
C++: CBufferList::Peek()
MPW-mangled: Peek__11CBufferListFv
*/
    JT _ZN11CBufferList4PeekEv, 01801c80

/*
C++: CBufferPipe::Peek(unsigned char)
MPW-mangled: Peek__11CBufferPipeFUc
*/
    JT _ZN11CBufferPipe4PeekEh, 01801c84

/*
C++: CRingBuffer::Peek()
MPW-mangled: Peek__11CRingBufferFv
*/
    JT _ZN11CRingBuffer4PeekEv, 01801c88

/*
C++: CBufferSegment::Peek()
MPW-mangled: Peek__14CBufferSegmentFv
*/
    JT _ZN14CBufferSegment4PeekEv, 01801c8c

/*
C++: CShadowRingBuffer::Peek()
MPW-mangled: Peek__17CShadowRingBufferFv
*/
    JT _ZN17CShadowRingBuffer4PeekEv, 01801c90

/*
C++: TDoubleQContainer::Peek()
MPW-mangled: Peek__17TDoubleQContainerFv
*/
    JT _ZN17TDoubleQContainer4PeekEv, 018012bc

/*
C++: CShadowBufferSegment::Peek()
MPW-mangled: Peek__20CShadowBufferSegmentFv
*/
    JT _ZN20CShadowBufferSegment4PeekEv, 01801c94

/*
C++: PointOnClipboard(const TPoint&)
MPW-mangled: PointOnClipboard__FRC6TPoint
*/
    JT _Z16PointOnClipboardRK6TPoint, 01800c70

/*
C++: TView::PointToCaret(TPoint&, TRect*, TRect*)
MPW-mangled: PointToCaret__5TViewFR6TPointP5TRectT2
*/
    JT PointToCaret__5TViewFR6TPointP5TRectT2, 01800c74

/*
C++: TInterpreter::PopValue()
MPW-mangled: PopValue__12TInterpreterFv
*/
    JT _ZN12TInterpreter8PopValueEv, 01801edc

/*
C++: CBufferList::Position() const
MPW-mangled: Position__11CBufferListCFv
*/
    JT _ZNK11CBufferList8PositionEv, 01801c98

/*
C++: CBufferSegment::Position() const
MPW-mangled: Position__14CBufferSegmentCFv
*/
    JT _ZNK14CBufferSegment8PositionEv, 01801c9c

/*
C++: CShadowBufferSegment::Position() const
MPW-mangled: Position__20CShadowBufferSegmentCFv
*/
    JT Position__20CShadowBufferSegmentCFv, 01801ca0

/*
C++: TCommTool::PostCommEvent(TCommToolGetEventReply&, long)
MPW-mangled: PostCommEvent__9TCommToolFR22TCommToolGetEventReplyl
*/
    JT _ZN9TCommTool13PostCommEventER22TCommToolGetEventReplyl, 01800708

/*
C++: TView::PostDraw(TRect&)
MPW-mangled: PostDraw__5TViewFR5TRect
*/
    JT _ZN5TView8PostDrawER5TRect, 01800c78

/*
C++: TSystemEventHandler::PowerOff(TAEvent*)
MPW-mangled: PowerOff__19TSystemEventHandlerFP7TAEvent
*/
    JT _ZN19TSystemEventHandler8PowerOffEP7TAEvent, 01801ca4

/*
C++: TSystemEventHandler::PowerOn(TAEvent*)
MPW-mangled: PowerOn__19TSystemEventHandlerFP7TAEvent
*/
    JT _ZN19TSystemEventHandler7PowerOnEP7TAEvent, 01801ca8

/*
C++: PrRegainControl(TPrinter*)
MPW-mangled: PrRegainControl__FP8TPrinter
*/
    JT _Z15PrRegainControlP8TPrinter, 01801618

/*
C++: PrReleaseControl(unsigned long, TPrinter*)
MPW-mangled: PrReleaseControl__FUlP8TPrinter
*/
    JT _Z16PrReleaseControlmP8TPrinter, 0180161c

/*
C++: TView::PreDraw(TRect&)
MPW-mangled: PreDraw__5TViewFR5TRect
*/
    JT _ZN5TView7PreDrawER5TRect, 01800c7c

/*
C++: TCommTool::PrepControlRequest(unsigned long)
MPW-mangled: PrepControlRequest__9TCommToolFUl
*/
    JT _ZN9TCommTool18PrepControlRequestEm, 0180070c

/*
C++: TCommTool::PrepGetRequest()
MPW-mangled: PrepGetRequest__9TCommToolFv
*/
    JT _ZN9TCommTool14PrepGetRequestEv, 01800710

/*
C++: TCommTool::PrepKillRequest()
MPW-mangled: PrepKillRequest__9TCommToolFv
*/
    JT _ZN9TCommTool15PrepKillRequestEv, 01800714

/*
C++: TCommTool::PrepPutRequest()
MPW-mangled: PrepPutRequest__9TCommToolFv
*/
    JT _ZN9TCommTool14PrepPutRequestEv, 01800718

/*
C++: static MemObjManager::PrimGetDomainInfoByName(unsigned long, DomainInfo*)
MPW-mangled: PrimGetDomainInfoByName__13MemObjManagerSFUlP10DomainInfo
*/
    JT _ZN13MemObjManager23PrimGetDomainInfoByNameEmP10DomainInfo, 018012c4

/*
C++: static MemObjManager::PrimGetDomainInfo(unsigned long, DomainInfo*)
MPW-mangled: PrimGetDomainInfo__13MemObjManagerSFUlP10DomainInfo
*/
    JT _ZN13MemObjManager17PrimGetDomainInfoEmP10DomainInfo, 018012c0

/*
C++: static MemObjManager::PrimGetEntryByIndex(MemObjType, unsigned long, void*)
MPW-mangled: PrimGetEntryByIndex__13MemObjManagerSF10MemObjTypeUlPv
*/
    JT _ZN13MemObjManager19PrimGetEntryByIndexE10MemObjTypemPv, 018012c8

/*
C++: static MemObjManager::PrimGetEntryByName(MemObjType, unsigned long, void*)
MPW-mangled: PrimGetEntryByName__13MemObjManagerSF10MemObjTypeUlPv
*/
    JT _ZN13MemObjManager18PrimGetEntryByNameE10MemObjTypemPv, 018012cc

/*
C++: static MemObjManager::PrimGetEnvDomainName(unsigned long, unsigned long, unsigned long*, unsigned char*, unsigned char*)
MPW-mangled: PrimGetEnvDomainName__13MemObjManagerSFUlT1PUlPUcT4
*/
    JT PrimGetEnvDomainName__13MemObjManagerSFUlT1PUlPUcT4, 018012d0

/*
C++: static MemObjManager::PrimGetEnvironmentInfo(unsigned long, EnvironmentInfo*)
MPW-mangled: PrimGetEnvironmentInfo__13MemObjManagerSFUlP15EnvironmentInfo
*/
    JT _ZN13MemObjManager22PrimGetEnvironmentInfoEmP15EnvironmentInfo, 018012d4

/*
C++: static TRealTimeClock::PrimRawSetAlarm(unsigned long)
MPW-mangled: PrimRawSetAlarm__14TRealTimeClockSFUl
*/
    JT _ZN14TRealTimeClock15PrimRawSetAlarmEm, 018012d8

/*
C++: static TRealTimeClock::PrimSetAlarm(unsigned long)
MPW-mangled: PrimSetAlarm__14TRealTimeClockSFUl
*/
    JT _ZN14TRealTimeClock12PrimSetAlarmEm, 018012dc

/*
C++: static MemObjManager::PrimSetEntryByIndex(MemObjType, unsigned long, void*)
MPW-mangled: PrimSetEntryByIndex__13MemObjManagerSF10MemObjTypeUlPv
*/
    JT _ZN13MemObjManager19PrimSetEntryByIndexE10MemObjTypemPv, 018012e0

/*
C++: static MemObjManager::PrimSetEntryByName(MemObjType, unsigned long, void*)
MPW-mangled: PrimSetEntryByName__13MemObjManagerSF10MemObjTypeUlPv
*/
    JT _ZN13MemObjManager18PrimSetEntryByNameE10MemObjTypemPv, 018012e4

/*
C++: PrintObject(const RefVar&, unsigned long)
MPW-mangled: PrintObject__FRC6RefVarUl
*/
    JT _Z11PrintObjectRK6RefVarm, 01802050

/*
C++: TSoupIndex::Prior(SKey*, SKey*, unsigned char, SKey*, SKey*)
MPW-mangled: Prior__10TSoupIndexFP4SKeyT1UcN21
*/
    JT Prior__10TSoupIndexFP4SKeyT1UcN21, 018009cc

/*
C++: TCommTool::ProcessOption(TOption*, unsigned long, unsigned long)
MPW-mangled: ProcessOption__9TCommToolFP7TOptionUlT2
*/
    JT ProcessOption__9TCommToolFP7TOptionUlT2, 0180071c

/*
C++: TCommTool::ProcessOptions(TOptionArray*)
MPW-mangled: ProcessOptions__9TCommToolFP12TOptionArray
*/
    JT _ZN9TCommTool14ProcessOptionsEP12TOptionArray, 01800720

/*
C++: ProtoEQ(const RefVar&, const RefVar&)
MPW-mangled: ProtoEQ__FRC6RefVarT1
*/
    JT ProtoEQ__FRC6RefVarT1, 01800c80
    JT PtrToHeap, 0180169c

/*
C++: PublicClearOldPatchInfo()
MPW-mangled: PublicClearOldPatchInfo__Fv
*/
    JT _Z23PublicClearOldPatchInfov, 01802cb4
    JT PublicEnterAtomic, 01801f10
    JT PublicEnterFIQAtomic, 01801f18
    JT PublicExitAtomic, 01801f14
    JT PublicExitFIQAtomic, 01801f1c
    JT PublicFiller_1, 0180248c
    JT PublicFiller_10, 01802468
    JT PublicFiller_100, 01802300
    JT PublicFiller_101, 018022fc
    JT PublicFiller_102, 018022f8
    JT PublicFiller_103, 018022f4
    JT PublicFiller_104, 018022f0
    JT PublicFiller_105, 018022ec
    JT PublicFiller_106, 018022e8
    JT PublicFiller_107, 018022e4
    JT PublicFiller_108, 018022e0
    JT PublicFiller_109, 018022dc
    JT PublicFiller_11, 01802464
    JT PublicFiller_110, 018022d8
    JT PublicFiller_111, 018022d4
    JT PublicFiller_112, 018022d0
    JT PublicFiller_113, 018022cc
    JT PublicFiller_114, 018022c8
    JT PublicFiller_115, 018022c4
    JT PublicFiller_116, 018022c0
    JT PublicFiller_117, 018022bc
    JT PublicFiller_118, 018022b8
    JT PublicFiller_119, 018022b4
    JT PublicFiller_12, 01802460
    JT PublicFiller_120, 018022b0
    JT PublicFiller_121, 018022ac
    JT PublicFiller_122, 018022a8
    JT PublicFiller_123, 018022a4
    JT PublicFiller_124, 018022a0
    JT PublicFiller_125, 0180229c
    JT PublicFiller_126, 01802298
    JT PublicFiller_127, 01802294
    JT PublicFiller_128, 01802290
    JT PublicFiller_129, 0180228c
    JT PublicFiller_13, 0180245c
    JT PublicFiller_130, 01802288
    JT PublicFiller_131, 01802284
    JT PublicFiller_132, 01802280
    JT PublicFiller_133, 0180227c
    JT PublicFiller_134, 01802278
    JT PublicFiller_135, 01802274
    JT PublicFiller_136, 01802270
    JT PublicFiller_137, 0180226c
    JT PublicFiller_138, 01802268
    JT PublicFiller_139, 01802264
    JT PublicFiller_14, 01802458
    JT PublicFiller_140, 01802260
    JT PublicFiller_141, 0180225c
    JT PublicFiller_142, 01802258
    JT PublicFiller_143, 01802254
    JT PublicFiller_144, 01802250
    JT PublicFiller_145, 0180224c
    JT PublicFiller_146, 01802248
    JT PublicFiller_147, 01802244
    JT PublicFiller_148, 01802240
    JT PublicFiller_149, 0180223c
    JT PublicFiller_15, 01802454
    JT PublicFiller_150, 01802238
    JT PublicFiller_151, 01802234
    JT PublicFiller_152, 01802230
    JT PublicFiller_153, 0180222c
    JT PublicFiller_154, 01802228
    JT PublicFiller_155, 01802224
    JT PublicFiller_156, 01802220
    JT PublicFiller_157, 0180221c
    JT PublicFiller_158, 01802218
    JT PublicFiller_159, 01802214
    JT PublicFiller_16, 01802450
    JT PublicFiller_160, 01802210
    JT PublicFiller_161, 0180220c
    JT PublicFiller_162, 01802208
    JT PublicFiller_163, 01802204
    JT PublicFiller_164, 01802200
    JT PublicFiller_165, 018021fc
    JT PublicFiller_166, 018021f8
    JT PublicFiller_167, 018021f4
    JT PublicFiller_168, 018021f0
    JT PublicFiller_169, 018021ec
    JT PublicFiller_17, 0180244c
    JT PublicFiller_170, 018021e8
    JT PublicFiller_171, 018021e4
    JT PublicFiller_172, 018021e0
    JT PublicFiller_173, 018021dc
    JT PublicFiller_174, 018021d8
    JT PublicFiller_175, 018021d4
    JT PublicFiller_176, 018021d0
    JT PublicFiller_177, 018021cc
    JT PublicFiller_178, 018021c8
    JT PublicFiller_179, 018021c4
    JT PublicFiller_18, 01802448
    JT PublicFiller_180, 018021c0
    JT PublicFiller_181, 018021bc
    JT PublicFiller_182, 018021b8
    JT PublicFiller_183, 018021b4
    JT PublicFiller_184, 018021b0
    JT PublicFiller_185, 018021ac
    JT PublicFiller_186, 018021a8
    JT PublicFiller_187, 018021a4
    JT PublicFiller_188, 018021a0
    JT PublicFiller_189, 0180219c
    JT PublicFiller_19, 01802444
    JT PublicFiller_190, 01802198
    JT PublicFiller_191, 01802194
    JT PublicFiller_192, 01802190
    JT PublicFiller_193, 0180218c
    JT PublicFiller_194, 01802188
    JT PublicFiller_195, 01802184
    JT PublicFiller_196, 01802180
    JT PublicFiller_197, 0180217c
    JT PublicFiller_198, 01802178
    JT PublicFiller_199, 01802174
    JT PublicFiller_2, 01802488
    JT PublicFiller_20, 01802440
    JT PublicFiller_200, 01802170
    JT PublicFiller_201, 0180216c
    JT PublicFiller_202, 01802168
    JT PublicFiller_203, 01802164
    JT PublicFiller_204, 01802160
    JT PublicFiller_205, 0180215c
    JT PublicFiller_206, 01802158
    JT PublicFiller_207, 01802154
    JT PublicFiller_208, 01802150
    JT PublicFiller_209, 0180214c
    JT PublicFiller_21, 0180243c
    JT PublicFiller_210, 01802148
    JT PublicFiller_211, 01802144
    JT PublicFiller_212, 01802140
    JT PublicFiller_213, 0180213c
    JT PublicFiller_214, 01802138
    JT PublicFiller_215, 01802134
    JT PublicFiller_216, 01802130
    JT PublicFiller_217, 0180212c
    JT PublicFiller_218, 01802128
    JT PublicFiller_219, 01802124
    JT PublicFiller_22, 01802438
    JT PublicFiller_220, 01802120
    JT PublicFiller_221, 0180211c
    JT PublicFiller_222, 01802118
    JT PublicFiller_223, 01802114
    JT PublicFiller_224, 01802110
    JT PublicFiller_225, 0180210c
    JT PublicFiller_226, 01802108
    JT PublicFiller_227, 01802104
    JT PublicFiller_228, 01802100
    JT PublicFiller_229, 018020fc
    JT PublicFiller_23, 01802434
    JT PublicFiller_230, 018020f8
    JT PublicFiller_231, 01800f54
    JT PublicFiller_235, 01800b50
    JT PublicFiller_236, 018020e0
    JT PublicFiller_238, 01801478
    JT PublicFiller_239, 01800fe0
    JT PublicFiller_24, 01802430
    JT PublicFiller_25, 0180242c
    JT PublicFiller_26, 01802428
    JT PublicFiller_260, 018025bc
    JT PublicFiller_261, 018025c0
    JT PublicFiller_262, 018025c4
    JT PublicFiller_263, 018025c8
    JT PublicFiller_264, 018025cc
    JT PublicFiller_265, 018025d0
    JT PublicFiller_266, 018025d4
    JT PublicFiller_267, 018025d8
    JT PublicFiller_268, 018025dc
    JT PublicFiller_269, 01800cfc
    JT PublicFiller_27, 01802424
    JT PublicFiller_270, 01801158
    JT PublicFiller_271, 018011b8
    JT PublicFiller_272, 018011d4
    JT PublicFiller_273, 01801208
    JT PublicFiller_274, 0180121c
    JT PublicFiller_275, 01801244
    JT PublicFiller_276, 018012e8
    JT PublicFiller_277, 018014a8
    JT PublicFiller_278, 01801568
    JT PublicFiller_279, 01801f78
    JT PublicFiller_28, 01802420
    JT PublicFiller_280, 01801f7c
    JT PublicFiller_281, 01801f90
    JT PublicFiller_282, 01801f94
    JT PublicFiller_283, 01801fc0
    JT PublicFiller_285, 0180201c
    JT PublicFiller_29, 0180241c
    JT PublicFiller_3, 01802484
    JT PublicFiller_30, 01802418
    JT PublicFiller_31, 01802414
    JT PublicFiller_32, 01802410
    JT PublicFiller_324, 01801f80
    JT PublicFiller_327, 01801f74
    JT PublicFiller_328, 01800ae0
    JT PublicFiller_329, 018009e4
    JT PublicFiller_33, 0180240c
    JT PublicFiller_330, 0180097c
    JT PublicFiller_331, 01800a10
    JT PublicFiller_332, 01800ce8
    JT PublicFiller_333, 01801bdc
    JT PublicFiller_334, 0180074c
    JT PublicFiller_335, 01800748
    JT PublicFiller_336, 01800744
    JT PublicFiller_337, 01800740
    JT PublicFiller_338, 0180073c
    JT PublicFiller_339, 01800738
    JT PublicFiller_34, 01802408
    JT PublicFiller_340, 01800688
    JT PublicFiller_341, 01800684
    JT PublicFiller_342, 01800680
    JT PublicFiller_343, 0180067c
    JT PublicFiller_344, 01800678
    JT PublicFiller_345, 01800674
    JT PublicFiller_346, 018014f0
    JT PublicFiller_347, 01800e90
    JT PublicFiller_348, 01800adc
    JT PublicFiller_349, 01800ad8
    JT PublicFiller_35, 01802404
    JT PublicFiller_350, 018024d0
    JT PublicFiller_351, 018024cc
    JT PublicFiller_36, 01802400
    JT PublicFiller_365, 018026c8
    JT PublicFiller_366, 018009ac
    JT PublicFiller_367, 01801f68
    JT PublicFiller_368, 01800bac
    JT PublicFiller_369, 01800b9c
    JT PublicFiller_37, 018023fc
    JT PublicFiller_370, 01800904
    JT PublicFiller_371, 018007b0
    JT PublicFiller_372, 01800848
    JT PublicFiller_373, 01800a20
    JT PublicFiller_374, 01800a1c
    JT PublicFiller_375, 01800930
    JT PublicFiller_376, 01800800
    JT PublicFiller_377, 0180126c
    JT PublicFiller_378, 01802564
    JT PublicFiller_379, 01800a08
    JT PublicFiller_38, 018023f8
    JT PublicFiller_380, 01800ccc
    JT PublicFiller_381, 01800d00
    JT PublicFiller_382, 01800cc4
    JT PublicFiller_383, 01800c58
    JT PublicFiller_384, 01800be4
    JT PublicFiller_385, 01800be0
    JT PublicFiller_386, 01800b80
    JT PublicFiller_387, 01800a58
    JT PublicFiller_388, 018004c8
    JT PublicFiller_389, 0180048c
    JT PublicFiller_39, 018023f4
    JT PublicFiller_390, 01800488
    JT PublicFiller_391, 01800484
    JT PublicFiller_392, 01800450
    JT PublicFiller_393, 0180044c
    JT PublicFiller_394, 018026c4
    JT PublicFiller_395, 01801dc8
    JT PublicFiller_396, 01801dc4
    JT PublicFiller_397, 01801dc0
    JT PublicFiller_398, 01801110
    JT PublicFiller_399, 01800a80
    JT PublicFiller_4, 01802480
    JT PublicFiller_40, 018023f0
    JT PublicFiller_400, 01800d20
    JT PublicFiller_401, 018016ec
    JT PublicFiller_402, 018016f0
    JT PublicFiller_403, 018016f4
    JT PublicFiller_404, 018016f8
    JT PublicFiller_405, 018016fc
    JT PublicFiller_406, 01801700
    JT PublicFiller_407, 01801704
    JT PublicFiller_408, 01801708
    JT PublicFiller_409, 0180170c
    JT PublicFiller_41, 018023ec
    JT PublicFiller_410, 01801710
    JT PublicFiller_411, 01801714
    JT PublicFiller_412, 01801718
    JT PublicFiller_413, 0180171c
    JT PublicFiller_414, 01801720
    JT PublicFiller_415, 01801724
    JT PublicFiller_416, 01801728
    JT PublicFiller_417, 0180172c
    JT PublicFiller_418, 01801730
    JT PublicFiller_419, 01801734
    JT PublicFiller_42, 018023e8
    JT PublicFiller_420, 01801738
    JT PublicFiller_421, 0180173c
    JT PublicFiller_422, 01801740
    JT PublicFiller_423, 01801744
    JT PublicFiller_424, 018010c4
    JT PublicFiller_425, 01800d60
    JT PublicFiller_43, 018023e4
    JT PublicFiller_44, 018023e0
    JT PublicFiller_45, 018023dc
    JT PublicFiller_46, 018023d8
    JT PublicFiller_47, 018023d4
    JT PublicFiller_48, 018023d0
    JT PublicFiller_49, 018023cc
    JT PublicFiller_5, 0180247c
    JT PublicFiller_50, 018023c8
    JT PublicFiller_51, 018023c4
    JT PublicFiller_52, 018023c0
    JT PublicFiller_53, 018023bc
    JT PublicFiller_54, 018023b8
    JT PublicFiller_55, 018023b4
    JT PublicFiller_56, 018023b0
    JT PublicFiller_57, 018023ac
    JT PublicFiller_58, 018023a8
    JT PublicFiller_59, 018023a4
    JT PublicFiller_6, 01802478
    JT PublicFiller_60, 018023a0
    JT PublicFiller_61, 0180239c
    JT PublicFiller_62, 01802398
    JT PublicFiller_63, 01802394
    JT PublicFiller_64, 01802390
    JT PublicFiller_65, 0180238c
    JT PublicFiller_66, 01802388
    JT PublicFiller_67, 01802384
    JT PublicFiller_68, 01802380
    JT PublicFiller_69, 0180237c
    JT PublicFiller_7, 01802474
    JT PublicFiller_70, 01802378
    JT PublicFiller_71, 01802374
    JT PublicFiller_72, 01802370
    JT PublicFiller_73, 0180236c
    JT PublicFiller_74, 01802368
    JT PublicFiller_75, 01802364
    JT PublicFiller_76, 01802360
    JT PublicFiller_77, 0180235c
    JT PublicFiller_78, 01802358
    JT PublicFiller_79, 01802354
    JT PublicFiller_8, 01802470
    JT PublicFiller_80, 01802350
    JT PublicFiller_81, 0180234c
    JT PublicFiller_82, 01802348
    JT PublicFiller_83, 01802344
    JT PublicFiller_84, 01802340
    JT PublicFiller_85, 0180233c
    JT PublicFiller_86, 01802338
    JT PublicFiller_87, 01802334
    JT PublicFiller_88, 01802330
    JT PublicFiller_89, 0180232c
    JT PublicFiller_9, 0180246c
    JT PublicFiller_90, 01802328
    JT PublicFiller_91, 01802324
    JT PublicFiller_92, 01802320
    JT PublicFiller_93, 0180231c
    JT PublicFiller_94, 01802318
    JT PublicFiller_95, 01802314
    JT PublicFiller_96, 01802310
    JT PublicFiller_97, 0180230c
    JT PublicFiller_98, 01802308
    JT PublicFiller_99, 01802304

/*
C++: TInterpreter::PushValue(const RefVar&)
MPW-mangled: PushValue__12TInterpreterFRC6RefVar
*/
    JT _ZN12TInterpreter9PushValueERK6RefVar, 01801ed8

/*
C++: TCommTool::PutComplete(long, unsigned long)
MPW-mangled: PutComplete__9TCommToolFlUl
*/
    JT _ZN9TCommTool11PutCompleteElm, 01800724

/*
C++: TCircleBuf::PutEOM(unsigned long)
MPW-mangled: PutEOM__10TCircleBufFUl
*/
    JT _ZN10TCircleBuf6PutEOMEm, 01802b34

/*
C++: TCircleBuf::PutFirstPossible(unsigned char)
MPW-mangled: PutFirstPossible__10TCircleBufFUc
*/
    JT _ZN10TCircleBuf16PutFirstPossibleEh, 01802a4c

/*
C++: TCircleBuf::PutNextByte(unsigned char)
MPW-mangled: PutNextByte__10TCircleBufFUc
*/
    JT _ZN10TCircleBuf11PutNextByteEh, 01802b38

/*
C++: TCircleBuf::PutNextByte(unsigned char, unsigned long)
MPW-mangled: PutNextByte__10TCircleBufFUcUl
*/
    JT _ZN10TCircleBuf11PutNextByteEhm, 01802b3c

/*
C++: TCircleBuf::PutNextCommit()
MPW-mangled: PutNextCommit__10TCircleBufFv
*/
    JT _ZN10TCircleBuf13PutNextCommitEv, 01802a50

/*
C++: TCircleBuf::PutNextEOM(unsigned long)
MPW-mangled: PutNextEOM__10TCircleBufFUl
*/
    JT _ZN10TCircleBuf10PutNextEOMEm, 01802a54

/*
C++: TCircleBuf::PutNextPossible(unsigned char)
MPW-mangled: PutNextPossible__10TCircleBufFUc
*/
    JT _ZN10TCircleBuf15PutNextPossibleEh, 01802a58

/*
C++: TCircleBuf::PutNextStart()
MPW-mangled: PutNextStart__10TCircleBufFv
*/
    JT _ZN10TCircleBuf12PutNextStartEv, 01802a5c

/*
C++: CBufferList::Put(int)
MPW-mangled: Put__11CBufferListFi
*/
    JT _ZN11CBufferList3PutEi, 01801cac

/*
C++: CBufferPipe::Put(int)
MPW-mangled: Put__11CBufferPipeFi
*/
    JT _ZN11CBufferPipe3PutEi, 01801cb0

/*
C++: CRingBuffer::Put(int)
MPW-mangled: Put__11CRingBufferFi
*/
    JT _ZN11CRingBuffer3PutEi, 01801cb4

/*
C++: CBufferSegment::Put(int)
MPW-mangled: Put__14CBufferSegmentFi
*/
    JT _ZN14CBufferSegment3PutEi, 01801cb8

/*
C++: CShadowRingBuffer::Put(int)
MPW-mangled: Put__17CShadowRingBufferFi
*/
    JT _ZN17CShadowRingBuffer3PutEi, 01801cbc

/*
C++: CShadowBufferSegment::Put(int)
MPW-mangled: Put__20CShadowBufferSegmentFi
*/
    JT _ZN20CShadowBufferSegment3PutEi, 01801cc0

/*
C++: CBufferList::Putn(const unsigned char*, long)
MPW-mangled: Putn__11CBufferListFPCUcl
*/
    JT _ZN11CBufferList4PutnEPKhl, 01801cc4

/*
C++: CRingBuffer::Putn(const unsigned char*, long)
MPW-mangled: Putn__11CRingBufferFPCUcl
*/
    JT _ZN11CRingBuffer4PutnEPKhl, 01801cc8

/*
C++: CBufferSegment::Putn(const unsigned char*, long)
MPW-mangled: Putn__14CBufferSegmentFPCUcl
*/
    JT _ZN14CBufferSegment4PutnEPKhl, 01801ccc

/*
C++: CShadowRingBuffer::Putn(const unsigned char*, long)
MPW-mangled: Putn__17CShadowRingBufferFPCUcl
*/
    JT _ZN17CShadowRingBuffer4PutnEPKhl, 01801cd0

/*
C++: CShadowBufferSegment::Putn(const unsigned char*, long)
MPW-mangled: Putn__20CShadowBufferSegmentFPCUcl
*/
    JT _ZN20CShadowBufferSegment4PutnEPKhl, 01801cd4

/*
C++: Query(const RefVar&, const RefVar&)
MPW-mangled: Query__FRC6RefVarT1
*/
    JT _Z5QueryRK6RefVarS1_, 018009d0
    JT QuickEnableInterrupt, 01802be0

/*
C++: RCHAR(long)
MPW-mangled: RCHAR__Fl
*/
    JT _Z5RCHARl, 018009d4

/*
C++: REPAcceptLine()
MPW-mangled: REPAcceptLine__Fv
*/
    JT _Z13REPAcceptLinev, 018009f8

/*
C++: REPIdle()
MPW-mangled: REPIdle__Fv
*/
    JT _Z7REPIdlev, 01800a00

/*
C++: REPInit()
MPW-mangled: REPInit__Fv
*/
    JT _Z7REPInitv, 01800a04

/*
C++: REPflush()
MPW-mangled: REPflush__Fv
*/
    JT _Z8REPflushv, 018009fc

/*
C++: ROMDomainBase()
MPW-mangled: ROMDomainBase__Fv
*/
    JT _Z13ROMDomainBasev, 018013d4

/*
C++: ROMDomainManagerFreePageCount()
MPW-mangled: ROMDomainManagerFreePageCount__Fv
*/
    JT _Z29ROMDomainManagerFreePageCountv, 018013d8

/*
C++: ROMDomainSize()
MPW-mangled: ROMDomainSize__Fv
*/
    JT _Z13ROMDomainSizev, 018013dc

/*
C++: ROMSoupBounds(char**, char**)
MPW-mangled: ROMSoupBounds__FPPcT1
*/
    JT ROMSoupBounds__FPPcT1, 01802cc4

/*
C++: TEndpointClient::RcvComplete(TEndpointEvent*)
MPW-mangled: RcvComplete__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient11RcvCompleteEP14TEndpointEvent, 018003b8

/*
C++: TEndpoint::Rcv(CBufferSegment*, long, unsigned long*, unsigned long)
MPW-mangled: Rcv__9TEndpointFP14CBufferSegmentlPUlUl
*/
    JT _ZN9TEndpoint3RcvEP14CBufferSegmentlPmm, 018003b0

/*
C++: TEndpoint::Rcv(unsigned char*, long&, long, unsigned long*, unsigned long)
MPW-mangled: Rcv__9TEndpointFPUcRllPUlUl
*/
    JT _ZN9TEndpoint3RcvEPhRllPmm, 018003b4

/*
C++: TCardCISIterator::ReadCIS(unsigned char*, unsigned char*, unsigned long, unsigned char)
MPW-mangled: ReadCIS__16TCardCISIteratorFPUcT1UlUc
*/
    JT ReadCIS__16TCardCISIteratorFPUcT1UlUc, 018024f8

/*
C++: CBufferPipe::ReadChunk(void*, long&, unsigned char&)
MPW-mangled: ReadChunk__11CBufferPipeFPvRlRUc
*/
    JT _ZN11CBufferPipe9ReadChunkEPvRlRh, 01801cd8

/*
C++: CPartPipe::ReadChunk(void*, long&, unsigned char&)
MPW-mangled: ReadChunk__9CPartPipeFPvRlRUc
*/
    JT _ZN9CPartPipe9ReadChunkEPvRlRh, 018012ec

/*
C++: CRingPipe::ReadChunk(void*, long&, unsigned char&)
MPW-mangled: ReadChunk__9CRingPipeFPvRlRUc
*/
    JT _ZN9CRingPipe9ReadChunkEPvRlRh, 01801cdc

/*
C++: TUPhys::ReadOnly(unsigned char&)
MPW-mangled: ReadOnly__6TUPhysFRUc
*/
    JT _ZN6TUPhys8ReadOnlyERh, 018012f0

/*
C++: CBufferPipe::ReadPosition() const
MPW-mangled: ReadPosition__11CBufferPipeCFv
*/
    JT ReadPosition__11CBufferPipeCFv, 01801ce0

/*
C++: CPartPipe::ReadPosition() const
MPW-mangled: ReadPosition__9CPartPipeCFv
*/
    JT ReadPosition__9CPartPipeCFv, 018012f4

/*
C++: CRingPipe::ReadPosition() const
MPW-mangled: ReadPosition__9CRingPipeCFv
*/
    JT ReadPosition__9CRingPipeCFv, 01801ce4

/*
C++: CBufferPipe::ReadSeek(long, int)
MPW-mangled: ReadSeek__11CBufferPipeFli
*/
    JT _ZN11CBufferPipe8ReadSeekEli, 01801ce8

/*
C++: CPartPipe::ReadSeek(long, int)
MPW-mangled: ReadSeek__9CPartPipeFli
*/
    JT _ZN9CPartPipe8ReadSeekEli, 018012f8

/*
C++: CRingPipe::ReadSeek(long, int)
MPW-mangled: ReadSeek__9CRingPipeFli
*/
    JT _ZN9CRingPipe8ReadSeekEli, 01801cec

/*
C++: TObjectReader::Read()
MPW-mangled: Read__13TObjectReaderFv
*/
    JT _ZN13TObjectReader4ReadEv, 018009d8

/*
C++: TATA::Read(unsigned char*, unsigned long, unsigned long, unsigned char, unsigned char)
MPW-mangled: Read__4TATAFPUcUlT2UcT4
*/
    JT Read__4TATAFPUcUlT2UcT4, 01802ab0

/*
C++: RealClockSeconds()
MPW-mangled: RealClockSeconds__Fv
*/
    JT _Z16RealClockSecondsv, 01802b7c

/*
C++: TView::RealDoCommand(TCommand*)
MPW-mangled: RealDoCommand__5TViewFP8TCommand
*/
    JT _ZN5TView13RealDoCommandEP8TCommand, 01800c84

/*
C++: TView::RealDraw(TRect&)
MPW-mangled: RealDraw__5TViewFR5TRect
*/
    JT _ZN5TView8RealDrawER5TRect, 01800c88
    JT ReallocPtr, 0180182c

/*
C++: Reboot(long, unsigned long, unsigned char)
MPW-mangled: Reboot__FlUlUc
*/
    JT _Z6Rebootlmh, 01801e64

/*
C++: TView::RecalcBounds()
MPW-mangled: RecalcBounds__5TViewFv
*/
    JT _ZN5TView12RecalcBoundsEv, 01800c8c

/*
C++: TUPort::Receive(TUAsyncMessage*, unsigned long, unsigned long, unsigned char)
MPW-mangled: Receive__6TUPortFP14TUAsyncMessageUlT2Uc
*/
    JT Receive__6TUPortFP14TUAsyncMessageUlT2Uc, 01801300

/*
C++: TUPort::Receive(unsigned long*, void*, unsigned long, TUMsgToken*, unsigned long*, unsigned long, unsigned long, unsigned char, unsigned char)
MPW-mangled: Receive__6TUPortFPUlPvUlP10TUMsgTokenT1N23UcT8
*/
    JT _ZN6TUPort7ReceiveEPmPvmP10TUMsgTokenS0_mmhh, 01801304

/*
C++: TNSDebugAPI::Receiver(long)
MPW-mangled: Receiver__11TNSDebugAPIFl
*/
    JT _ZN11TNSDebugAPI8ReceiverEl, 01802804

/*
C++: RefToInt(const RefVar&)
MPW-mangled: RefToInt__FRC6RefVar
*/
    JT _Z8RefToIntRK6RefVar, 018029d8

/*
C++: RefToUniChar(const RefVar&)
MPW-mangled: RefToUniChar__FRC6RefVar
*/
    JT _Z12RefToUniCharRK6RefVar, 018029dc

/*
C++: RegTaskTemplate(const RefVar&, const RefVar&)
MPW-mangled: RegTaskTemplate__FRC6RefVarT1
*/
    JT RegTaskTemplate__FRC6RefVarT1, 01800784

/*
C++: RegisterAdditionalInterrupt(InterruptObject**, unsigned long, void*, long (*)(void*), unsigned long)
MPW-mangled: RegisterAdditionalInterrupt__FPP15InterruptObjectUlPvPFPv_lT2
*/
    JT RegisterAdditionalInterrupt__FPP15InterruptObjectUlPvPFPv_lT2, 01802be4

/*
C++: RegisterClass(char*, char*)
MPW-mangled: RegisterClass__FPcT1
*/
    JT RegisterClass__FPcT1, 018009dc
    JT RegisterDelayedFunctionGlue, 0180131c

/*
C++: static MemObjManager::RegisterDomainId(unsigned long, unsigned long)
MPW-mangled: RegisterDomainId__13MemObjManagerSFUlT1
*/
    JT RegisterDomainId__13MemObjManagerSFUlT1, 01801320

/*
C++: static MemObjManager::RegisterEntryByName(MemObjType, unsigned long, void*)
MPW-mangled: RegisterEntryByName__13MemObjManagerSF10MemObjTypeUlPv
*/
    JT _ZN13MemObjManager19RegisterEntryByNameE10MemObjTypemPv, 01801324

/*
C++: static MemObjManager::RegisterEnvironmentId(unsigned long, unsigned long)
MPW-mangled: RegisterEnvironmentId__13MemObjManagerSFUlT1
*/
    JT RegisterEnvironmentId__13MemObjManagerSFUlT1, 01801328

/*
C++: RegisterFaultMonitor(unsigned long, unsigned long, unsigned long)
MPW-mangled: RegisterFaultMonitor__FUlN21
*/
    JT RegisterFaultMonitor__FUlN21, 0180132c

/*
C++: TSystemEvent::RegisterForSystemEvent(unsigned long, unsigned long, unsigned long)
MPW-mangled: RegisterForSystemEvent__12TSystemEventFUlN21
*/
    JT RegisterForSystemEvent__12TSystemEventFUlN21, 01801330

/*
C++: TUGestalt::RegisterGestalt(unsigned long, void*, unsigned long)
MPW-mangled: RegisterGestalt__9TUGestaltFUlPvT1
*/
    JT _ZN9TUGestalt15RegisterGestaltEmPvm, 01802548

/*
C++: static MemObjManager::RegisterHeapRef(unsigned long, void*)
MPW-mangled: RegisterHeapRef__13MemObjManagerSFUlPv
*/
    JT _ZN13MemObjManager15RegisterHeapRefEmPv, 01801334

/*
C++: RegisterInterrupt(InterruptObject**, unsigned long, void*, long (*)(void*), unsigned long)
MPW-mangled: RegisterInterrupt__FPP15InterruptObjectUlPvPFPv_lT2
*/
    JT RegisterInterrupt__FPP15InterruptObjectUlPvPFPv_lT2, 01802be8

/*
C++: RegisterInterrupt(unsigned long, void*, long (*)(void*), unsigned long)
MPW-mangled: RegisterInterrupt__FUlPvPFPv_lT1
*/
    JT _Z17RegisterInterruptmPvPFlS_Em, 01801338

/*
C++: RegisterL2Interrupt(InterruptObject**, unsigned long, void*, long (*)(void*), unsigned long, long (*)(void*))
MPW-mangled: RegisterL2Interrupt__FPP15InterruptObjectUlPvPFPv_lT2T4
*/
    JT RegisterL2Interrupt__FPP15InterruptObjectUlPvPFPv_lT2T4, 01802bec

/*
C++: RegisterL2Interrupt(unsigned long, void*, long (*)(void*), long (*)(void*), unsigned long)
MPW-mangled: RegisterL2Interrupt__FUlPvPFPv_lT3T1
*/
    JT RegisterL2Interrupt__FUlPvPFPv_lT3T1, 018027c4

/*
C++: TUNameServer::RegisterName(char*, char*, unsigned long, unsigned long)
MPW-mangled: RegisterName__12TUNameServerFPcT1UlT3
*/
    JT _ZN12TUNameServer12RegisterNameEPcS0_mm, 0180133c
    JT _ZN12TUNameServer12RegisterNameEPKcS1_mm, 0180133c  /* const char* variant */

/*
C++: static TUDomainManager::RegisterPageMonitor()
MPW-mangled: RegisterPageMonitor__15TUDomainManagerSFv
*/
    JT _ZN15TUDomainManager19RegisterPageMonitorEv, 01801340

/*
C++: RegisterPatch(void*, unsigned long, unsigned long, long)
MPW-mangled: RegisterPatch__FPvUlT2l
*/
    JT RegisterPatch__FPvUlT2l, 01801344

/*
C++: RegisterROMDomainManager()
MPW-mangled: RegisterROMDomainManager__Fv
*/
    JT _Z24RegisterROMDomainManagerv, 0180134c

/*
C++: RegisterRealTimeClockHandler(void*, long (*)(void*))
MPW-mangled: RegisterRealTimeClockHandler__FPvPFPv_l
*/
    JT RegisterRealTimeClockHandler__FPvPFPv_l, 01801348

/*
C++: TCardSocket::RegisterSocketInterrupt(TSocketInt, (*)(void*, TCardSocket*, long, void*))
MPW-mangled: RegisterSocketInterrupt__11TCardSocketF10TSocketIntPFPvP11TCardSocket_lPv
*/
    JT RegisterSocketInterrupt__11TCardSocketF10TSocketIntPFPvP11TCardSocket_lPv, 01801350

/*
C++: RegisterTStore(TStore*)
MPW-mangled: RegisterTStore__FP6TStore
*/
    JT _Z14RegisterTStoreP6TStore, 018009e0

/*
C++: RegisterTricInterrupt(unsigned long, void*, long (*)(void*))
MPW-mangled: RegisterTricInterrupt__FUlPvPFPv_l
*/
    JT RegisterTricInterrupt__FUlPvPFPv_l, 01801354

/*
C++: TClassInfo::Register() const
MPW-mangled: Register__10TClassInfoCFv
*/
    JT _ZNK10TClassInfo8RegisterEv, 01801308

/*
C++: TPartHandler::Register()
MPW-mangled: Register__12TPartHandlerFv
*/
    JT _ZN12TPartHandler8RegisterEv, 0180130c

/*
C++: static TUPageManager::Register(unsigned long)
MPW-mangled: Register__13TUPageManagerSFUl
*/
    JT _ZN13TUPageManager8RegisterEm, 01801310

/*
C++: static TRealTimeClock::Register(unsigned long)
MPW-mangled: Register__14TRealTimeClockSFUl
*/
    JT _ZN14TRealTimeClock8RegisterEm, 01801314

/*
C++: TClassInfoRegistry::Register(const TClassInfo*, unsigned long)
MPW-mangled: Register__18TClassInfoRegistryFPC10TClassInfoUl
*/
    JT _ZN18TClassInfoRegistry8RegisterEPK10TClassInfom, 01801318

/*
C++: TPartHandler::RejectPart()
MPW-mangled: RejectPart__12TPartHandlerFv
*/
    JT _ZN12TPartHandler10RejectPartEv, 01801358

/*
C++: TWRecognizer::RejectUnit(TWRecUnit*)
MPW-mangled: RejectUnit__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer10RejectUnitEP9TWRecUnit, 01802d4c

/*
C++: TEndpointClient::ReleaseComplete(TEndpointEvent*)
MPW-mangled: ReleaseComplete__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient15ReleaseCompleteEP14TEndpointEvent, 018003c4

/*
C++: TCommTool::ReleaseComplete(long)
MPW-mangled: ReleaseComplete__9TCommToolFl
*/
    JT _ZN9TCommTool15ReleaseCompleteEl, 0180072c

/*
C++: static TUDomainManager::ReleasePageTable(unsigned long)
MPW-mangled: ReleasePageTable__15TUDomainManagerSFUl
*/
    JT _ZN15TUDomainManager16ReleasePageTableEm, 0180257c

/*
C++: ReleasePage(unsigned long)
MPW-mangled: ReleasePage__FUl
*/
    JT _Z11ReleasePagem, 01801368

/*
C++: TCurrentBank::ReleasePower(unsigned long, TSocketPowerLevels, unsigned long)
MPW-mangled: ReleasePower__12TCurrentBankFUl18TSocketPowerLevelsT1
*/
    JT ReleasePower__12TCurrentBankFUl18TSocketPowerLevelsT1, 01802ab4

/*
C++: TURdWrSemaphore::ReleaseRd()
MPW-mangled: ReleaseRd__15TURdWrSemaphoreFv
*/
    JT _ZN15TURdWrSemaphore9ReleaseRdEv, 0180136c

/*
C++: static TUPageManagerMonitor::ReleaseRequest(unsigned long, int)
MPW-mangled: ReleaseRequest__20TUPageManagerMonitorSFUli
*/
    JT _ZN20TUPageManagerMonitor14ReleaseRequestEmi, 01801370

/*
C++: TCommTool::ReleaseStart()
MPW-mangled: ReleaseStart__9TCommToolFv
*/
    JT _ZN9TCommTool12ReleaseStartEv, 01800730

/*
C++: TURdWrSemaphore::ReleaseWr()
MPW-mangled: ReleaseWr__15TURdWrSemaphoreFv
*/
    JT _ZN15TURdWrSemaphore9ReleaseWrEv, 01801374

/*
C++: static TUPageManager::Release(unsigned long)
MPW-mangled: Release__13TUPageManagerSFUl
*/
    JT _ZN13TUPageManager7ReleaseEm, 0180135c

/*
C++: TEndpointClient::Release(TEndpointEvent*)
MPW-mangled: Release__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient7ReleaseEP14TEndpointEvent, 018003bc

/*
C++: static TUDomainManager::Release(unsigned long)
MPW-mangled: Release__15TUDomainManagerSFUl
*/
    JT _ZN15TUDomainManager7ReleaseEm, 01801360

/*
C++: TULockingSemaphore::Release()
MPW-mangled: Release__18TULockingSemaphoreFv
*/
    JT _ZN18TULockingSemaphore7ReleaseEv, 01801364

/*
C++: TCommTool::Release()
MPW-mangled: Release__9TCommToolFv
*/
    JT _ZN9TCommTool7ReleaseEv, 01800728

/*
C++: TEndpoint::Release(unsigned long)
MPW-mangled: Release__9TEndpointFUl
*/
    JT _ZN9TEndpoint7ReleaseEm, 018003c0

/*
C++: RelocateFramesInPage(FrameRelocationHeader*, char*, long)
MPW-mangled: RelocateFramesInPage__FP21FrameRelocationHeaderPcl
*/
    JT _Z20RelocateFramesInPageP21FrameRelocationHeaderPcl, 01801e50

/*
C++: TUDomainManager::RememberPermMap(unsigned long, unsigned long, Perm)
MPW-mangled: RememberPermMap__15TUDomainManagerFUlT14Perm
*/
    JT RememberPermMap__15TUDomainManagerFUlT14Perm, 01801380

/*
C++: static TUDomainManager::RememberPermMap(unsigned long, unsigned long, unsigned long, Perm)
MPW-mangled: RememberPermMap__15TUDomainManagerSFUlN214Perm
*/
    JT RememberPermMap__15TUDomainManagerSFUlN214Perm, 01801384

/*
C++: TUDomainManager::RememberPhysMapRange(unsigned long, unsigned long, unsigned long, unsigned long, unsigned char)
MPW-mangled: RememberPhysMapRange__15TUDomainManagerFUlN31Uc
*/
    JT RememberPhysMapRange__15TUDomainManagerFUlN31Uc, 0180256c

/*
C++: static TUDomainManager::RememberPhysMapRange(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned char)
MPW-mangled: RememberPhysMapRange__15TUDomainManagerSFUlN41Uc
*/
    JT RememberPhysMapRange__15TUDomainManagerSFUlN41Uc, 01802570

/*
C++: TUDomainManager::RememberPhysMap(unsigned long, unsigned long, unsigned char)
MPW-mangled: RememberPhysMap__15TUDomainManagerFUlT1Uc
*/
    JT RememberPhysMap__15TUDomainManagerFUlT1Uc, 01801388

/*
C++: static TUDomainManager::RememberPhysMap(unsigned long, unsigned long, unsigned long, unsigned char)
MPW-mangled: RememberPhysMap__15TUDomainManagerSFUlN21Uc
*/
    JT RememberPhysMap__15TUDomainManagerSFUlN21Uc, 0180138c

/*
C++: TUDomainManager::Remember(unsigned long, unsigned long, unsigned long, unsigned char)
MPW-mangled: Remember__15TUDomainManagerFUlN21Uc
*/
    JT Remember__15TUDomainManagerFUlN21Uc, 01801378

/*
C++: static TUDomainManager::Remember(unsigned long, unsigned long, unsigned long, unsigned long, unsigned char)
MPW-mangled: Remember__15TUDomainManagerSFUlN31Uc
*/
    JT Remember__15TUDomainManagerSFUlN31Uc, 0180137c

/*
C++: TCardATALoader::RemoveATAPackages(TATABootParamBlock*, void*, unsigned long)
MPW-mangled: RemoveATAPackages__14TCardATALoaderFP18TATABootParamBlockPvUl
*/
    JT _ZN14TCardATALoader17RemoveATAPackagesEP18TATABootParamBlockPvm, 01802ab8

/*
C++: TOptionArray::RemoveAllOptions()
MPW-mangled: RemoveAllOptions__12TOptionArrayFv
*/
    JT _ZN12TOptionArray16RemoveAllOptionsEv, 018003c8

/*
C++: TView::RemoveAllViews()
MPW-mangled: RemoveAllViews__5TViewFv
*/
    JT _ZN5TView14RemoveAllViewsEv, 01800c90

/*
C++: CBufferList::RemoveAll()
MPW-mangled: RemoveAll__11CBufferListFv
*/
    JT _ZN11CBufferList9RemoveAllEv, 01801cf8

/*
C++: CBufferList::RemoveAt(long)
MPW-mangled: RemoveAt__11CBufferListFl
*/
    JT _ZN11CBufferList8RemoveAtEl, 01801cfc

/*
C++: TView::RemoveChildView(TView*)
MPW-mangled: RemoveChildView__5TViewFP5TView
*/
    JT _ZN5TView15RemoveChildViewEP5TView, 01800c94

/*
C++: RemoveDomainFromEnvironment(unsigned long, unsigned long)
MPW-mangled: RemoveDomainFromEnvironment__FUlT1
*/
    JT RemoveDomainFromEnvironment__FUlT1, 018013a4

/*
C++: CDynamicArray::RemoveElementsAt(long, long)
MPW-mangled: RemoveElementsAt__13CDynamicArrayFlT1
*/
    JT RemoveElementsAt__13CDynamicArrayFlT1, 01801d00

/*
C++: CArrayIterator::RemoveElementsAt(long, long)
MPW-mangled: RemoveElementsAt__14CArrayIteratorFlT1
*/
    JT RemoveElementsAt__14CArrayIteratorFlT1, 01801d04
    JT RemoveExceptionHandler, 01801830

/*
C++: TCardPCMCIA::RemoveFields()
MPW-mangled: RemoveFields__11TCardPCMCIAFv
*/
    JT _ZN11TCardPCMCIA12RemoveFieldsEv, 018013a8

/*
C++: CBufferList::RemoveFirst()
MPW-mangled: RemoveFirst__11CBufferListFv
*/
    JT _ZN11CBufferList11RemoveFirstEv, 01801d08

/*
C++: TEndpointPipe::RemoveFromAppWorld()
MPW-mangled: RemoveFromAppWorld__13TEndpointPipeFv
*/
    JT _ZN13TEndpointPipe18RemoveFromAppWorldEv, 018003cc

/*
C++: TEndpointEventHandler::RemoveFromAppWorld()
MPW-mangled: RemoveFromAppWorld__21TEndpointEventHandlerFv
*/
    JT _ZN21TEndpointEventHandler18RemoveFromAppWorldEv, 018003d0

/*
C++: TEndpoint::RemoveFromAppWorld()
MPW-mangled: RemoveFromAppWorld__9TEndpointFv
*/
    JT _ZN9TEndpoint18RemoveFromAppWorldEv, 018003d4

/*
C++: TDoubleQContainer::RemoveFromQueue(void*)
MPW-mangled: RemoveFromQueue__17TDoubleQContainerFPv
*/
    JT _ZN17TDoubleQContainer15RemoveFromQueueEPv, 018013ac

/*
C++: TView::RemoveFromSoup(TView*)
MPW-mangled: RemoveFromSoup__5TViewFP5TView
*/
    JT _ZN5TView14RemoveFromSoupEP5TView, 01800c98

/*
C++: RemoveGCRoot(long&)
MPW-mangled: RemoveGCRoot__FRl
*/
    JT _Z12RemoveGCRootRl, 018009ec

/*
C++: CBufferList::RemoveLast()
MPW-mangled: RemoveLast__11CBufferListFv
*/
    JT _ZN11CBufferList10RemoveLastEv, 01801d0c

/*
C++: TOptionArray::RemoveOptionAt(long)
MPW-mangled: RemoveOptionAt__12TOptionArrayFl
*/
    JT _ZN12TOptionArray14RemoveOptionAtEl, 018003d8

/*
C++: TOptionIterator::RemoveOptionAt(long)
MPW-mangled: RemoveOptionAt__15TOptionIteratorFl
*/
    JT _ZN15TOptionIterator14RemoveOptionAtEl, 018003dc

/*
C++: RemovePackage(unsigned long)
MPW-mangled: RemovePackage__FUl
*/
    JT _Z13RemovePackagem, 018013b0

/*
C++: RemoveSlot(const RefVar.global RemoveSlot__FRC6RefVarT1, const RefVar.global RemoveSlot__FRC6RefVarT1)
MPW-mangled: RemoveSlot__FRC6RefVarT1
*/
    JT _Z10RemoveSlotRK6RefVarS1_, 018009f0

/*
C++: RemoveTStore(TStore*)
MPW-mangled: RemoveTStore__FP6TStore
*/
    JT _Z12RemoveTStoreP6TStore, 018009f4

/*
C++: TView::RemoveUnmarked()
MPW-mangled: RemoveUnmarked__5TViewFv
*/
    JT _ZN5TView14RemoveUnmarkedEv, 01800c9c

/*
C++: TView::RemoveView()
MPW-mangled: RemoveView__5TViewFv
*/
    JT _ZN5TView10RemoveViewEv, 01800ca0

/*
C++: CBufferList::Remove(CBuffer*)
MPW-mangled: Remove__11CBufferListFP7CBuffer
*/
    JT _ZN11CBufferList6RemoveEP7CBuffer, 01801cf0

/*
C++: TPartHandler::Remove(TPkPartRemoveEvent*)
MPW-mangled: Remove__12TPartHandlerFP18TPkPartRemoveEvent
*/
    JT _ZN12TPartHandler6RemoveEP18TPkPartRemoveEvent, 01801390

/*
C++: TUEnvironment::Remove(unsigned long)
MPW-mangled: Remove__13TUEnvironmentFUl
*/
    JT _ZN13TUEnvironment6RemoveEm, 01801394

/*
C++: TDoubleQContainer::Remove()
MPW-mangled: Remove__17TDoubleQContainerFv
*/
    JT _ZN17TDoubleQContainer6RemoveEv, 01801398

/*
C++: TFramePartHandler::Remove(const PartId&, unsigned long, long)
MPW-mangled: Remove__17TFramePartHandlerFRC6PartIdUll
*/
    JT _ZN17TFramePartHandler6RemoveERK6PartIdml, 018009e8

/*
C++: CList::Remove(void*)
MPW-mangled: Remove__5CListFPv
*/
    JT _ZN5CList6RemoveEPv, 01801cf4

/*
C++: TUCTTable::Remove(TTask*)
MPW-mangled: Remove__9TUCTTableFP5TTask
*/
    JT _ZN9TUCTTable6RemoveEP5TTask, 0180139c

/*
C++: TUCTTable::Remove(unsigned long)
MPW-mangled: Remove__9TUCTTableFUl
*/
    JT _ZN9TUCTTable6RemoveEm, 018013a0

/*
C++: TView::ReorderView(TView*, long)
MPW-mangled: ReorderView__5TViewFP5TViewl
*/
    JT _ZN5TView11ReorderViewEP5TViewl, 01800ca4

/*
C++: CList::ReplaceAt(long, void*)
MPW-mangled: ReplaceAt__5CListFlPv
*/
    JT _ZN5CList9ReplaceAtElPv, 01801d14

/*
C++: CDynamicArray::ReplaceElementsAt(long, void*, long)
MPW-mangled: ReplaceElementsAt__13CDynamicArrayFlPvT1
*/
    JT ReplaceElementsAt__13CDynamicArrayFlPvT1, 01801d18

/*
C++: TUGestalt::ReplaceGestalt(unsigned long, void*, unsigned long)
MPW-mangled: ReplaceGestalt__9TUGestaltFUlPvT1
*/
    JT ReplaceGestalt__9TUGestaltFUlPvT1, 0180254c

/*
C++: ReplaceObjectRef(long, long)
MPW-mangled: ReplaceObjectRef__FlT1
*/
    JT _Z16ReplaceObjectRefll, 01800a0c

/*
C++: ReplaceObject(const RefVar&, const RefVar&)
MPW-mangled: ReplaceObject__FRC6RefVarT1
*/
    JT ReplaceObject__FRC6RefVarT1, 018029e0

/*
C++: CList::Replace(void*, void*)
MPW-mangled: Replace__5CListFPvT1
*/
    JT Replace__5CListFPvT1, 01801d10

/*
C++: TPartHandler::ReplyImmed(long)
MPW-mangled: ReplyImmed__12TPartHandlerFl
*/
    JT _ZN12TPartHandler10ReplyImmedEl, 018013b4

/*
C++: TAEventHandler::ReplyImmed()
MPW-mangled: ReplyImmed__14TAEventHandlerFv
*/
    JT _ZN14TAEventHandler10ReplyImmedEv, 01801d1c

/*
C++: TUMsgToken::ReplyRPC(void*, unsigned long, long)
MPW-mangled: ReplyRPC__10TUMsgTokenFPvUll
*/
    JT _ZN10TUMsgToken8ReplyRPCEPvml, 018013b8

/*
C++: ReportMemMgrTrap(long)
MPW-mangled: ReportMemMgrTrap__Fl
*/
    JT _Z16ReportMemMgrTrapl, 01801834

/*
C++: ReportSmashedHeap(char*, long, void*)
MPW-mangled: ReportSmashedHeap__FPclPv
*/
    JT _Z17ReportSmashedHeapPclPv, 01801838

/*
C++: TCardSocket::RequestPower(TSocketPowerLevels, unsigned long)
MPW-mangled: RequestPower__11TCardSocketF18TSocketPowerLevelsUl
*/
    JT _ZN11TCardSocket12RequestPowerE18TSocketPowerLevelsm, 01802abc

/*
C++: TCommTool::RequestTypeToChannelNumber(CommToolRequestType)
MPW-mangled: RequestTypeToChannelNumber__9TCommToolF19CommToolRequestType
*/
    JT _ZN9TCommTool26RequestTypeToChannelNumberE19CommToolRequestType, 01800734

/*
C++: ResetAccountTimeKernelGlue()
MPW-mangled: ResetAccountTimeKernelGlue__Fv
*/
    JT _Z26ResetAccountTimeKernelGluev, 018013c4

/*
C++: CArrayIterator::ResetBounds(unsigned char)
MPW-mangled: ResetBounds__14CArrayIteratorFUc
*/
    JT _ZN14CArrayIterator11ResetBoundsEh, 01801d50

/*
C++: TOptionIterator::ResetBounds()
MPW-mangled: ResetBounds__15TOptionIteratorFv
*/
    JT _ZN15TOptionIterator11ResetBoundsEv, 018003ec

/*
C++: TCardCISIterator::ResetCIS()
MPW-mangled: ResetCIS__16TCardCISIteratorFv
*/
    JT _ZN16TCardCISIterator8ResetCISEv, 01802d98

/*
C++: TCardCISIterator::ResetFields()
MPW-mangled: ResetFields__16TCardCISIteratorFv
*/
    JT _ZN16TCardCISIterator11ResetFieldsEv, 018024f0

/*
C++: TAEventHandler::ResetIdle(unsigned long, TimeUnits)
MPW-mangled: ResetIdle__14TAEventHandlerFUl9TimeUnits
*/
    JT _ZN14TAEventHandler9ResetIdleEm9TimeUnits, 01801d54

/*
C++: TAEventHandler::ResetIdle(unsigned long, TimeUnits, TUPort*)
MPW-mangled: ResetIdle__14TAEventHandlerFUl9TimeUnitsP6TUPort
*/
    JT _ZN14TAEventHandler9ResetIdleEm9TimeUnitsP6TUPort, 01801f00

/*
C++: TAEventHandler::ResetIdle()
MPW-mangled: ResetIdle__14TAEventHandlerFv
*/
    JT _ZN14TAEventHandler9ResetIdleEv, 01801d58

/*
C++: CBufferList::ResetMark()
MPW-mangled: ResetMark__11CBufferListFv
*/
    JT _ZN11CBufferList9ResetMarkEv, 01801d5c

/*
C++: ResetStackStateBlock(StackState*)
MPW-mangled: ResetStackStateBlock__FP10StackState
*/
    JT _Z20ResetStackStateBlockP10StackState, 0180272c

/*
C++: TCircleBuf::ResetStart()
MPW-mangled: ResetStart__10TCircleBufFv
*/
    JT _ZN10TCircleBuf10ResetStartEv, 01802b40

/*
C++: TDelayTimer::ResetTimeOut(unsigned long)
MPW-mangled: ResetTimeOut__11TDelayTimerFUl
*/
    JT _ZN11TDelayTimer12ResetTimeOutEm, 018025ac

/*
C++: TCircleBuf::Reset()
MPW-mangled: Reset__10TCircleBufFv
*/
    JT _ZN10TCircleBuf5ResetEv, 01802b44

/*
C++: TIdleTimer::Reset(unsigned long, TimeUnits)
MPW-mangled: Reset__10TIdleTimerFUl9TimeUnits
*/
    JT _ZN10TIdleTimer5ResetEm9TimeUnits, 01801d20

/*
C++: TIdleTimer::Reset(unsigned long, TimeUnits, TUPort*)
MPW-mangled: Reset__10TIdleTimerFUl9TimeUnitsP6TUPort
*/
    JT _ZN10TIdleTimer5ResetEm9TimeUnitsP6TUPort, 01801f08

/*
C++: TIdleTimer::Reset()
MPW-mangled: Reset__10TIdleTimerFv
*/
    JT _ZN10TIdleTimer5ResetEv, 01801d24

/*
C++: CBufferList::Reset()
MPW-mangled: Reset__11CBufferListFv
*/
    JT _ZN11CBufferList5ResetEv, 01801d28

/*
C++: CBufferPipe::Reset()
MPW-mangled: Reset__11CBufferPipeFv
*/
    JT _ZN11CBufferPipe5ResetEv, 01801d2c

/*
C++: CRingBuffer::Reset()
MPW-mangled: Reset__11CRingBufferFv
*/
    JT _ZN11CRingBuffer5ResetEv, 01801d30

/*
C++: TOptionArray::Reset()
MPW-mangled: Reset__12TOptionArrayFv
*/
    JT _ZN12TOptionArray5ResetEv, 018003e0

/*
C++: CArrayIterator::Reset()
MPW-mangled: Reset__14CArrayIteratorFv
*/
    JT _ZN14CArrayIterator5ResetEv, 01801d34

/*
C++: CBufferSegment::Reset()
MPW-mangled: Reset__14CBufferSegmentFv
*/
    JT _ZN14CBufferSegment5ResetEv, 01801d38

/*
C++: TPackageLoader::Reset()
MPW-mangled: Reset__14TPackageLoaderFv
*/
    JT _ZN14TPackageLoader5ResetEv, 018013bc

/*
C++: TObjectIterator::Reset(const RefVar&)
MPW-mangled: Reset__15TObjectIteratorFRC6RefVar
*/
    JT _ZN15TObjectIterator5ResetERK6RefVar, 01800a14

/*
C++: TOptionIterator::Reset()
MPW-mangled: Reset__15TOptionIteratorFv
*/
    JT _ZN15TOptionIterator5ResetEv, 018003e4

/*
C++: CShadowRingBuffer::Reset()
MPW-mangled: Reset__17CShadowRingBufferFv
*/
    JT _ZN17CShadowRingBuffer5ResetEv, 01801d3c

/*
C++: TAEHandlerIterator::Reset()
MPW-mangled: Reset__18TAEHandlerIteratorFv
*/
    JT _ZN18TAEHandlerIterator5ResetEv, 01801d40

/*
C++: CShadowBufferSegment::Reset()
MPW-mangled: Reset__20CShadowBufferSegmentFv
*/
    JT _ZN20CShadowBufferSegment5ResetEv, 01801d44

/*
C++: TATA::Reset(unsigned char)
MPW-mangled: Reset__4TATAFUc
*/
    JT _ZN4TATA5ResetEh, 01802ac0

/*
C++: TOption::Reset()
MPW-mangled: Reset__7TOptionFv
*/
    JT _ZN7TOption5ResetEv, 018003e8

/*
C++: CNullPipe::Reset()
MPW-mangled: Reset__9CNullPipeFv
*/
    JT _ZN9CNullPipe5ResetEv, 01801d48

/*
C++: CPartPipe::Reset()
MPW-mangled: Reset__9CPartPipeFv
*/
    JT _ZN9CPartPipe5ResetEv, 018013c0

/*
C++: CRingPipe::Reset()
MPW-mangled: Reset__9CRingPipeFv
*/
    JT _ZN9CRingPipe5ResetEv, 01801d4c

/*
C++: RestartTimerOverflowDetect(void*)
MPW-mangled: RestartTimerOverflowDetect__FPv
*/
    JT _Z26RestartTimerOverflowDetectPv, 018013cc

/*
C++: Restart()
MPW-mangled: Restart__Fv
*/
    JT _Z7Restartv, 018013c8

/*
C++: RestoreVppState()
MPW-mangled: RestoreVppState__Fv
*/
    JT _Z15RestoreVppStatev, 018013d0

/*
C++: TATA::ResumeService(TCardSocket*, TCardPCMCIA*, unsigned long)
MPW-mangled: ResumeService__4TATAFP11TCardSocketP11TCardPCMCIAUl
*/
    JT _ZN4TATA13ResumeServiceEP11TCardSocketP11TCardPCMCIAm, 01802ac4
    JT ResurrectVMHeap, 0180183c

/*
C++: TNSDebugAPI::Return(long, const RefVar&)
MPW-mangled: Return__11TNSDebugAPIFlRC6RefVar
*/
    JT _ZN11TNSDebugAPI6ReturnElRK6RefVar, 01802834

/*
C++: TView::RunCacheScript(long, const RefVar&, unsigned char, unsigned char*)
MPW-mangled: RunCacheScript__5TViewFlRC6RefVarUcPUc
*/
    JT _ZN5TView14RunCacheScriptElRK6RefVarhPh, 01800ca8

/*
C++: RunModemNavigator(TOptionArray*)
MPW-mangled: RunModemNavigator__FP12TOptionArray
*/
    JT _Z17RunModemNavigatorP12TOptionArray, 01801f6c

/*
C++: TView::RunScript(const RefVar&, const RefVar&, unsigned char, unsigned char*)
MPW-mangled: RunScript__5TViewFRC6RefVarT1UcPUc
*/
    JT RunScript__5TViewFRC6RefVarT1UcPUc, 01800cac

/*
C++: TInterpreter::Run()
MPW-mangled: Run__12TInterpreterFv
*/
    JT _ZN12TInterpreter3RunEv, 01801ee0

/*
C++: CDynamicArray::SafeElementPtrAt(long)
MPW-mangled: SafeElementPtrAt__13CDynamicArrayFl
*/
    JT _ZN13CDynamicArray16SafeElementPtrAtEl, 01801d60

/*
C++: TCardATALoader::SameStrings(char*, char*, unsigned long)
MPW-mangled: SameStrings__14TCardATALoaderFPcT1Ul
*/
    JT SameStrings__14TCardATALoaderFPcT1Ul, 01802ac8

/*
C++: Satisfies(const TClassInfo*, const char*, const char*, unsigned long)
MPW-mangled: Satisfies__FPC10TClassInfoPCcT2Ul
*/
    JT Satisfies__FPC10TClassInfoPCcT2Ul, 018013e0

/*
C++: TClassInfoRegistry::Satisfy(const char*, const char*, unsigned long) const
MPW-mangled: Satisfy__18TClassInfoRegistryCFPCcT1Ul
*/
    JT Satisfy__18TClassInfoRegistryCFPCcT1Ul, 018013e4

/*
C++: TUSoundChannel::Schedule(SoundBlock*, TUSoundCallback*)
MPW-mangled: Schedule__14TUSoundChannelFP10SoundBlockP15TUSoundCallback
*/
    JT _ZN14TUSoundChannel8ScheduleEP10SoundBlockP15TUSoundCallback, 01802c48

/*
C++: TSoupIndex::Search(int, SKey*, SKey*, int (*)(SKey*, SKey*, void*), void*, SKey*, SKey*)
MPW-mangled: Search__10TSoupIndexFiP4SKeyT2PFP4SKeyT1Pv_iPvN22
*/
    JT Search__10TSoupIndexFiP4SKeyT2PFP4SKeyT1Pv_iPvN22, 01800a18

/*
C++: CSortedList::Search(CItemTester*, long&)
MPW-mangled: Search__11CSortedListFP11CItemTesterRl
*/
    JT _ZN11CSortedList6SearchEP11CItemTesterRl, 01801d64

/*
C++: CList::Search(CItemTester*, long&)
MPW-mangled: Search__5CListFP11CItemTesterRl
*/
    JT _ZN5CList6SearchEP11CItemTesterRl, 01801d68

/*
C++: TClassInfoRegistry::Seed() const
MPW-mangled: Seed__18TClassInfoRegistryCFv
*/
    JT Seed__18TClassInfoRegistryCFv, 018013e8

/*
C++: CPartPipe::SeekEOF()
MPW-mangled: SeekEOF__9CPartPipeFv
*/
    JT _ZN9CPartPipe7SeekEOFEv, 018013ec

/*
C++: CBufferList::Seek(long, int)
MPW-mangled: Seek__11CBufferListFli
*/
    JT _ZN11CBufferList4SeekEli, 01801d6c

/*
C++: CBufferSegment::Seek(long, int)
MPW-mangled: Seek__14CBufferSegmentFli
*/
    JT _ZN14CBufferSegment4SeekEli, 01801d70

/*
C++: CShadowBufferSegment::Seek(long, int)
MPW-mangled: Seek__20CShadowBufferSegmentFli
*/
    JT _ZN20CShadowBufferSegment4SeekEli, 01801d74

/*
C++: TCardCISIterator::SelectCIS(unsigned long)
MPW-mangled: SelectCIS__16TCardCISIteratorFUl
*/
    JT _ZN16TCardCISIterator9SelectCISEm, 01802da0

/*
C++: TCardSocket::SelectIOInterface()
MPW-mangled: SelectIOInterface__11TCardSocketFv
*/
    JT _ZN11TCardSocket17SelectIOInterfaceEv, 018013f0

/*
C++: TCardSocket::SelectMemoryInterface()
MPW-mangled: SelectMemoryInterface__11TCardSocketFv
*/
    JT _ZN11TCardSocket21SelectMemoryInterfaceEv, 018013f4

/*
C++: TView::SelectNone()
MPW-mangled: SelectNone__5TViewFv
*/
    JT _ZN5TView10SelectNoneEv, 01800cb4

/*
C++: TCardSocket::SelectPCMCIABus()
MPW-mangled: SelectPCMCIABus__11TCardSocketFv
*/
    JT _ZN11TCardSocket15SelectPCMCIABusEv, 018013fc

/*
C++: TCardSocket::SelectVoltageLevel(TSocketPowerLevels)
MPW-mangled: SelectVoltageLevel__11TCardSocketF18TSocketPowerLevels
*/
    JT _ZN11TCardSocket18SelectVoltageLevelE18TSocketPowerLevels, 01802acc

/*
C++: TView::Select(unsigned char, unsigned char)
MPW-mangled: Select__5TViewFUcT1
*/
    JT Select__5TViewFUcT1, 01800cb0

/*
C++: TClassInfo::Selector() const
MPW-mangled: Selector__10TClassInfoCFv
*/
    JT Selector__10TClassInfoCFv, 018013f8

/*
C++: SemGroupGetRefCon(unsigned long, void**)
MPW-mangled: SemGroupGetRefCon__FUlPPv
*/
    JT _Z17SemGroupGetRefConmPPv, 01801400

/*
C++: SemGroupSetRefCon(unsigned long, void*)
MPW-mangled: SemGroupSetRefCon__FUlPv
*/
    JT _Z17SemGroupSetRefConmPv, 01801404

/*
C++: TUSemaphoreGroup::SemOp(TUSemaphoreOpList*, SemFlags)
MPW-mangled: SemOp__16TUSemaphoreGroupFP17TUSemaphoreOpList8SemFlags
*/
    JT _ZN16TUSemaphoreGroup5SemOpEP17TUSemaphoreOpList8SemFlags, 01801408

/*
C++: TUSemaphoreGroup::SemOp(unsigned long, SemFlags)
MPW-mangled: SemOp__16TUSemaphoreGroupFUl8SemFlags
*/
    JT _ZN16TUSemaphoreGroup5SemOpEm8SemFlags, 0180140c

/*
C++: SendForInterrupt(unsigned long, unsigned long, unsigned long, void*, unsigned long, unsigned long, unsigned long, TTime*, unsigned char)
MPW-mangled: SendForInterrupt__FUlN21PvN31P5TTimeUc
*/
    JT _Z16SendForInterruptmmmPvmmmP5TTimeh, 01801410

/*
C++: TUPort::SendGoo(unsigned long, unsigned long, void*, unsigned long, unsigned long, unsigned long, unsigned char, unsigned long, TTime*)
MPW-mangled: SendGoo__6TUPortFUlT1PvN31UcT1P5TTime
*/
    JT _ZN6TUPort7SendGooEmmPvmmmhmP5TTime, 01801414

/*
C++: TUPort::SendRPCGoo(unsigned long, unsigned long, unsigned long*, void*, unsigned long, unsigned long, unsigned long, unsigned char, void*, unsigned long, unsigned long, TTime*)
MPW-mangled: SendRPCGoo__6TUPortFUlT1PUlPvN31UcT4N21P5TTime
*/
    JT _ZN6TUPort10SendRPCGooEmmPmPvmmmhS1_mmP5TTime, 01801418

/*
C++: SendRPC(TAEventHandler*, TUPort*, TUAsyncMessage*, void*, unsigned long, void*, unsigned long, unsigned long, TTime*, unsigned long, unsigned char)
MPW-mangled: SendRPC__FP14TAEventHandlerP6TUPortP14TUAsyncMessagePvUlT4N25P5TTimeT5Uc
*/
    JT SendRPC__FP14TAEventHandlerP6TUPortP14TUAsyncMessagePvUlT4N25P5TTimeT5Uc, 01801d78

/*
C++: TATPInterface::SendRequest(unsigned char, TAddress*, unsigned long, unsigned long, unsigned char, unsigned char, unsigned long, unsigned long, unsigned char)
MPW-mangled: SendRequest__13TATPInterfaceFUcP8TAddressUlT3N21N23T1
*/
    JT SendRequest__13TATPInterfaceFUcP8TAddressUlT3N21N23T1, 01802098

/*
C++: SendRunScriptEvent(char*, char*, char*, long, long*)
MPW-mangled: SendRunScriptEvent__FPcN21lPl
*/
    JT SendRunScriptEvent__FPcN21lPl, 018002f8

/*
C++: TSendSystemEvent::SendSystemEvent(TUAsyncMessage*, void*, unsigned long, void*, unsigned long)
MPW-mangled: SendSystemEvent__16TSendSystemEventFP14TUAsyncMessagePvUlT2T3
*/
    JT SendSystemEvent__16TSendSystemEventFP14TUAsyncMessagePvUlT2T3, 0180141c

/*
C++: TSendSystemEvent::SendSystemEvent(void*, unsigned long)
MPW-mangled: SendSystemEvent__16TSendSystemEventFPvUl
*/
    JT _ZN16TSendSystemEvent15SendSystemEventEPvm, 01801420

/*
C++: TInterpreter::Send(const RefVar&, const RefVar&, const RefVar&, long)
MPW-mangled: Send__12TInterpreterFRC6RefVarN21l
*/
    JT Send__12TInterpreterFRC6RefVarN21l, 01801ecc

/*
C++: ServiceToPort(unsigned long, TUPort*)
MPW-mangled: ServiceToPort__FUlP6TUPort
*/
    JT _Z13ServiceToPortmP6TUPort, 018003f0

/*
C++: TAddress::SetAddress(unsigned long, unsigned short, unsigned char, unsigned char)
MPW-mangled: SetAddress__8TAddressFUlUsUcT3
*/
    JT SetAddress__8TAddressFUlUsUcT3, 018020ac

/*
C++: static TRealTimeClock::SetAlarm(unsigned long, TTime, unsigned long, unsigned long, void*, long, unsigned long)
MPW-mangled: SetAlarm__14TRealTimeClockSFUl5TTimeN21PvlT1
*/
    JT SetAlarm__14TRealTimeClockSFUl5TTimeN21PvlT1, 01801424

/*
C++: static TRealTimeClock::SetAlarm(unsigned long, unsigned long, long (*)(void*), void*, unsigned long, unsigned long)
MPW-mangled: SetAlarm__14TRealTimeClockSFUlT1PFPv_lPvN21
*/
    JT SetAlarm__14TRealTimeClockSFUlT1PFPv_lPvN21, 018024a8

/*
C++: static TURealTimeAlarm::SetAlarm(unsigned long, TTime, unsigned long, unsigned long, void*, long, unsigned long)
MPW-mangled: SetAlarm__15TURealTimeAlarmSFUl5TTimeN21PvlT1
*/
    JT SetAlarm__15TURealTimeAlarmSFUl5TTimeN21PvlT1, 01801428

/*
C++: static TURealTimeAlarm::SetAlarm(unsigned long, TTime, long (*)(void*), void*, unsigned long, unsigned long)
MPW-mangled: SetAlarm__15TURealTimeAlarmSFUl5TTimePFPv_lPvN21
*/
    JT SetAlarm__15TURealTimeAlarmSFUl5TTimePFPv_lPvN21, 01802c1c

/*
C++: TObjectReader::SetAllowFunctions(unsigned char)
MPW-mangled: SetAllowFunctions__13TObjectReaderFUc
*/
    JT _ZN13TObjectReader17SetAllowFunctionsEh, 01802e08
    JT SetAndClearBitsAtomic, 0180142c

/*
C++: CDynamicArray::SetArraySize(long)
MPW-mangled: SetArraySize__13CDynamicArrayFl
*/
    JT _ZN13CDynamicArray12SetArraySizeEl, 01801d7c

/*
C++: SetArraySlotRef(long, long, long)
MPW-mangled: SetArraySlotRef__FlN21
*/
    JT _Z15SetArraySlotReflll, 01800a24

/*
C++: SetArraySlot(const RefVar&, long, const RefVar&)
MPW-mangled: SetArraySlot__FRC6RefVarlT1
*/
    JT _Z12SetArraySlotRK6RefVarlS1_, 018029e4

/*
C++: TOption::SetAsAddress(unsigned long)
MPW-mangled: SetAsAddress__7TOptionFUl
*/
    JT _ZN7TOption12SetAsAddressEm, 018003f4

/*
C++: TOption::SetAsConfig(unsigned long)
MPW-mangled: SetAsConfig__7TOptionFUl
*/
    JT _ZN7TOption11SetAsConfigEm, 018003f8

/*
C++: TOption::SetAsOption(unsigned long)
MPW-mangled: SetAsOption__7TOptionFUl
*/
    JT _ZN7TOption11SetAsOptionEm, 018003fc

/*
C++: TOptionExtended::SetAsServiceSpecific(unsigned long)
MPW-mangled: SetAsServiceSpecific__15TOptionExtendedFUl
*/
    JT _ZN15TOptionExtended20SetAsServiceSpecificEm, 01802904

/*
C++: TOption::SetAsService(unsigned long)
MPW-mangled: SetAsService__7TOptionFUl
*/
    JT _ZN7TOption12SetAsServiceEm, 01800400

/*
C++: TOption::SetAsService()
MPW-mangled: SetAsService__7TOptionFv
*/
    JT _ZN7TOption12SetAsServiceEv, 01802b10

/*
C++: TCardSocket::SetAttributeMemSpeed(unsigned long)
MPW-mangled: SetAttributeMemSpeed__11TCardSocketFUl
*/
    JT _ZN11TCardSocket20SetAttributeMemSpeedEm, 01801430

/*
C++: TATA::SetAttributes(unsigned long)
MPW-mangled: SetAttributes__4TATAFUl
*/
    JT _ZN4TATA13SetAttributesEm, 01802ad0

/*
C++: SetBatteryTable(BatteryTable*)
MPW-mangled: SetBatteryTable__FP12BatteryTable
*/
    JT _Z15SetBatteryTableP12BatteryTable, 01801f84

/*
C++: SetBequeathId(unsigned long)
MPW-mangled: SetBequeathId__FUl
*/
    JT _Z13SetBequeathIdm, 01801434

/*
C++: TView::SetBounds(const TRect&)
MPW-mangled: SetBounds__5TViewFRC5TRect
*/
    JT _ZN5TView9SetBoundsERK5TRect, 01800cb8

/*
C++: TInterpreter::SetBreakPoints(const RefVar&)
MPW-mangled: SetBreakPoints__12TInterpreterFRC6RefVar
*/
    JT _ZN12TInterpreter14SetBreakPointsERK6RefVar, 01801ff4

/*
C++: TUSharedMem::SetBuffer(void*, unsigned long, unsigned long)
MPW-mangled: SetBuffer__11TUSharedMemFPvUlT2
*/
    JT SetBuffer__11TUSharedMemFPvUlT2, 01801438

/*
C++: TCardSocket::SetBusTimer(unsigned long)
MPW-mangled: SetBusTimer__11TCardSocketFUl
*/
    JT _ZN11TCardSocket11SetBusTimerEm, 0180143c

/*
C++: TCardPackage::SetCPUType(char*)
MPW-mangled: SetCPUType__12TCardPackageFPc
*/
    JT _ZN12TCardPackage10SetCPUTypeEPc, 01801468

/*
C++: TInterpreter::SetCallEnv()
MPW-mangled: SetCallEnv__12TInterpreterFv
*/
    JT _ZN12TInterpreter10SetCallEnvEv, 01802740

/*
C++: TUSoundCallbackProc::SetCallback(void (*)(SoundBlock*, int, long))
MPW-mangled: SetCallback__19TUSoundCallbackProcFPFP10SoundBlockil_v
*/
    JT SetCallback__19TUSoundCallbackProcFPFP10SoundBlockil_v, 01802c4c

/*
C++: TCardPCMCIA::SetCardManufacturer(char*)
MPW-mangled: SetCardManufacturer__11TCardPCMCIAFPc
*/
    JT _ZN11TCardPCMCIA19SetCardManufacturerEPc, 01801440

/*
C++: TCardPCMCIA::SetCardProduct(char*)
MPW-mangled: SetCardProduct__11TCardPCMCIAFPc
*/
    JT _ZN11TCardPCMCIA14SetCardProductEPc, 01801444

/*
C++: TCardSocket::SetCardServerPort(unsigned long)
MPW-mangled: SetCardServerPort__11TCardSocketFUl
*/
    JT _ZN11TCardSocket17SetCardServerPortEm, 01801448

/*
C++: TCardPCMCIA::SetCardV1String3(char*)
MPW-mangled: SetCardV1String3__11TCardPCMCIAFPc
*/
    JT _ZN11TCardPCMCIA16SetCardV1String3EPc, 0180144c

/*
C++: TCardPCMCIA::SetCardV1String4(char*)
MPW-mangled: SetCardV1String4__11TCardPCMCIAFPc
*/
    JT _ZN11TCardPCMCIA16SetCardV1String4EPc, 01801450

/*
C++: TCardPCMCIA::SetCardV2Info(char*)
MPW-mangled: SetCardV2Info__11TCardPCMCIAFPc
*/
    JT _ZN11TCardPCMCIA13SetCardV2InfoEPc, 01801454

/*
C++: TCardPCMCIA::SetCardV2Vendor(char*)
MPW-mangled: SetCardV2Vendor__11TCardPCMCIAFPc
*/
    JT _ZN11TCardPCMCIA15SetCardV2VendorEPc, 01801458

/*
C++: TCommTool::SetChannelFilter(CommToolRequestType, unsigned char)
MPW-mangled: SetChannelFilter__9TCommToolF19CommToolRequestTypeUc
*/
    JT _ZN9TCommTool16SetChannelFilterE19CommToolRequestTypeh, 01800750

/*
C++: TWRecognizer::SetCharWordString(TWRecUnit*, unsigned long, char*)
MPW-mangled: SetCharWordString__12TWRecognizerFP9TWRecUnitUlPc
*/
    JT _ZN12TWRecognizer17SetCharWordStringEP9TWRecUnitmPc, 01802d50

/*
C++: TView::SetChildrenVertical(long, long)
MPW-mangled: SetChildrenVertical__5TViewFlT1
*/
    JT SetChildrenVertical__5TViewFlT1, 01800cbc

/*
C++: SetClass(const RefVar&, const RefVar&)
MPW-mangled: SetClass__FRC6RefVarT1
*/
    JT SetClass__FRC6RefVarT1, 01800a28

/*
C++: TEndpoint::SetClientHandler(unsigned long)
MPW-mangled: SetClientHandler__9TEndpointFUl
*/
    JT _ZN9TEndpoint16SetClientHandlerEm, 01800404

/*
C++: TUAsyncMessage::SetCollectorPort(unsigned long)
MPW-mangled: SetCollectorPort__14TUAsyncMessageFUl
*/
    JT _ZN14TUAsyncMessage16SetCollectorPortEm, 0180145c

/*
C++: TCardSocket::SetCommonMemSpeed(unsigned long)
MPW-mangled: SetCommonMemSpeed__11TCardSocketFUl
*/
    JT _ZN11TCardSocket17SetCommonMemSpeedEm, 01801460

/*
C++: TCardSocket::SetControl(unsigned long)
MPW-mangled: SetControl__11TCardSocketFUl
*/
    JT _ZN11TCardSocket10SetControlEm, 01801464

/*
C++: TInterpreter::SetControl(long, long)
MPW-mangled: SetControl__12TInterpreterFlT1
*/
    JT SetControl__12TInterpreterFlT1, 01802008
    JT SetCurrentHeap, 01801840

/*
C++: TDate::SetCurrentTime()
MPW-mangled: SetCurrentTime__5TDateFv
*/
    JT _ZN5TDate14SetCurrentTimeEv, 01802638

/*
C++: TView::SetCustomPattern(const RefVar&)
MPW-mangled: SetCustomPattern__5TViewFRC6RefVar
*/
    JT _ZN5TView16SetCustomPatternERK6RefVar, 01800cc0

/*
C++: TCardSocket::SetDMAWatchTimer(unsigned long)
MPW-mangled: SetDMAWatchTimer__11TCardSocketFUl
*/
    JT _ZN11TCardSocket16SetDMAWatchTimerEm, 01801474

/*
C++: TCardSocket::SetDefaultConfig()
MPW-mangled: SetDefaultConfig__11TCardSocketFv
*/
    JT _ZN11TCardSocket16SetDefaultConfigEv, 0180146c

/*
C++: TCardSocket::SetDefaultSpeeds()
MPW-mangled: SetDefaultSpeeds__11TCardSocketFv
*/
    JT _ZN11TCardSocket16SetDefaultSpeedsEv, 01801470

/*
C++: TATA::SetDeviceControlReg(unsigned char)
MPW-mangled: SetDeviceControlReg__4TATAFUc
*/
    JT _ZN4TATA19SetDeviceControlRegEh, 01802ad4

/*
C++: SetDomainRange(unsigned long, unsigned long, unsigned long)
MPW-mangled: SetDomainRange__FUlN21
*/
    JT SetDomainRange__FUlN21, 0180147c

/*
C++: SetEnvironment(unsigned long, unsigned long*)
MPW-mangled: SetEnvironment__FUlPUl
*/
    JT _Z14SetEnvironmentmPm, 01801480

/*
C++: TSystemEvent::SetEvent(unsigned long)
MPW-mangled: SetEvent__12TSystemEventFUl
*/
    JT _ZN12TSystemEvent8SetEventEm, 01801484

/*
C++: SetExceptionHandler(CatchHeader*)
MPW-mangled: SetExceptionHandler__FP11CatchHeader
*/
    JT _Z19SetExceptionHandlerP11CatchHeader, 01801844

/*
C++: TUDomain::SetFaultMonitor(unsigned long)
MPW-mangled: SetFaultMonitor__8TUDomainFUl
*/
    JT _ZN8TUDomain15SetFaultMonitorEm, 01801488

/*
C++: SetFaultState(TProcessorState*)
MPW-mangled: SetFaultState__FP15TProcessorState
*/
    JT _Z13SetFaultStateP15TProcessorState, 0180148c

/*
C++: TATA::SetFeatures(unsigned char, unsigned char, unsigned char)
MPW-mangled: SetFeatures__4TATAFUcN21
*/
    JT SetFeatures__4TATAFUcN21, 01802ad8

/*
C++: TAppWorld::SetFilter(unsigned long)
MPW-mangled: SetFilter__9TAppWorldFUl
*/
    JT _ZN9TAppWorld9SetFilterEm, 01801d80

/*
C++: TNSDebugAPI::SetFindVar(long, const RefVar&, const RefVar&)
MPW-mangled: SetFindVar__11TNSDebugAPIFlRC6RefVarT2
*/
    JT SetFindVar__11TNSDebugAPIFlRC6RefVarT2, 01802824
    JT SetFixedHeap, 018016a0

/*
C++: TView::SetFlags(unsigned long)
MPW-mangled: SetFlags__5TViewFUl
*/
    JT _ZN5TView8SetFlagsEm, 01800cc8

/*
C++: TDate::SetFormatResource(const RefVar&)
MPW-mangled: SetFormatResource__5TDateFRC6RefVar
*/
    JT _ZN5TDate17SetFormatResourceERK6RefVar, 0180263c

/*
C++: TDate::SetFormatResource(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: SetFormatResource__5TDateFRC6RefVarN21
*/
    JT SetFormatResource__5TDateFRC6RefVarN21, 01802640

/*
C++: SetFramePath(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: SetFramePath__FRC6RefVarN21
*/
    JT SetFramePath__FRC6RefVarN21, 01800a2c

/*
C++: TFramePartHandler::SetFrameRemoveObject(const RefVar&)
MPW-mangled: SetFrameRemoveObject__17TFramePartHandlerFRC6RefVar
*/
    JT _ZN17TFramePartHandler20SetFrameRemoveObjectERK6RefVar, 01800a30

/*
C++: SetFrameSlot(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: SetFrameSlot__FRC6RefVarN21
*/
    JT _Z12SetFrameSlotRK6RefVarS1_S1_, 01800a34

/*
C++: TNSDebugAPI::SetFunction(long, const RefVar&)
MPW-mangled: SetFunction__11TNSDebugAPIFlRC6RefVar
*/
    JT _ZN11TNSDebugAPI11SetFunctionElRK6RefVar, 018027f8
    JT SetHandleName, 018016a4
    JT SetHandleOwner, 018016a8
    JT SetHandleSize, 01801848
    JT SetHandleType, 018016ac
    JT SetHeap, 0180184c
    JT SetHeapIsVMBacked, 018016b0
    JT SetHeapLimits, 01801490
    JT SetHeapRefcon, 01801850

/*
C++: TCardSocket::SetIOSpeed(unsigned long)
MPW-mangled: SetIOSpeed__11TCardSocketFUl
*/
    JT _ZN11TCardSocket10SetIOSpeedEm, 01801494

/*
C++: TNSDebugAPI::SetImplementor(long, const RefVar&)
MPW-mangled: SetImplementor__11TNSDebugAPIFlRC6RefVar
*/
    JT _ZN11TNSDebugAPI14SetImplementorElRK6RefVar, 01802810

/*
C++: TWRecognizer::SetLabel(TWRecUnit*, unsigned long, unsigned long)
MPW-mangled: SetLabel__12TWRecognizerFP9TWRecUnitUlT2
*/
    JT SetLabel__12TWRecognizerFP9TWRecUnitUlT2, 01802d54

/*
C++: SetLength(const RefVar&, long)
MPW-mangled: SetLength__FRC6RefVarl
*/
    JT _Z9SetLengthRK6RefVarl, 01800a38

/*
C++: SetLexScope(const RefVar&, const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: SetLexScope__FRC6RefVarN31
*/
    JT SetLexScope__FRC6RefVarN31, 01802754

/*
C++: TInterpreter::SetLocalOnStack(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: SetLocalOnStack__12TInterpreterFRC6RefVarN21
*/
    JT SetLocalOnStack__12TInterpreterFRC6RefVarN21, 01802010
    JT SetMemMgrBreak, 01801854

/*
C++: SetMemory(void*, long, char)
MPW-mangled: SetMemory__FPvlc
*/
    JT _Z9SetMemoryPvlc, 01801858

/*
C++: TFaxToolInterface::SetMinScanLineTime(unsigned long)
MPW-mangled: SetMinScanLineTime__17TFaxToolInterfaceFUl
*/
    JT _ZN17TFaxToolInterface18SetMinScanLineTimeEm, 01800754

/*
C++: TUSharedMemMsg::SetMsgAvailPort(unsigned long)
MPW-mangled: SetMsgAvailPort__14TUSharedMemMsgFUl
*/
    JT _ZN14TUSharedMemMsg15SetMsgAvailPortEm, 01801498

/*
C++: TATA::SetMultipleMode(unsigned char, unsigned char)
MPW-mangled: SetMultipleMode__4TATAFUcT1
*/
    JT SetMultipleMode__4TATAFUcT1, 01802adc

/*
C++: TCardPackage::SetName(char*)
MPW-mangled: SetName__12TCardPackageFPc
*/
    JT _ZN12TCardPackage7SetNameEPc, 0180149c

/*
C++: SetNetworkPersistentInfo(unsigned long)
MPW-mangled: SetNetworkPersistentInfo__FUl
*/
    JT _Z24SetNetworkPersistentInfom, 018014a0

/*
C++: TCardPackage::SetOSType(char*)
MPW-mangled: SetOSType__12TCardPackageFPc
*/
    JT _ZN12TCardPackage9SetOSTypeEPc, 018014a4

/*
C++: TView::SetOrigin(TPoint&)
MPW-mangled: SetOrigin__5TViewFR6TPoint
*/
    JT _ZN5TView9SetOriginER6TPoint, 01800cd0

/*
C++: TNSDebugAPI::SetPC(long, long)
MPW-mangled: SetPC__11TNSDebugAPIFlT1
*/
    JT SetPC__11TNSDebugAPIFlT1, 01802800

/*
C++: SetPattern(long)
MPW-mangled: SetPattern__Fl
*/
    JT _Z10SetPatternl, 01800cd4

/*
C++: TServiceInfo::SetPortId(unsigned long)
MPW-mangled: SetPortId__12TServiceInfoFUl
*/
    JT _ZN12TServiceInfo9SetPortIdEm, 01800408

/*
C++: SetPort(GrafPort*)
MPW-mangled: SetPort__FP8GrafPort
*/
    JT _Z7SetPortP8GrafPort, 01801e78

/*
C++: TATA::SetPowerMode(unsigned char, unsigned char, unsigned char)
MPW-mangled: SetPowerMode__4TATAFUcN21
*/
    JT SetPowerMode__4TATAFUcN21, 01802ae0
    JT SetPtrName, 018016b4
    JT SetPtrOwner, 018016b8
    JT SetPtrType, 018016bc

/*
C++: TCardSocket::SetPullupControl(unsigned long)
MPW-mangled: SetPullupControl__11TCardSocketFUl
*/
    JT _ZN11TCardSocket16SetPullupControlEm, 01802df0

/*
C++: TCardSocket::SetRdWrQueueControl(unsigned long)
MPW-mangled: SetRdWrQueueControl__11TCardSocketFUl
*/
    JT _ZN11TCardSocket19SetRdWrQueueControlEm, 01802dec

/*
C++: SetRealTimeClockAlarm(unsigned long)
MPW-mangled: SetRealTimeClockAlarm__FUl
*/
    JT _Z21SetRealTimeClockAlarmm, 018014b0

/*
C++: SetRealTimeClock(unsigned long)
MPW-mangled: SetRealTimeClock__FUl
*/
    JT _Z16SetRealTimeClockm, 018014ac

/*
C++: TNSDebugAPI::SetReceiver(long, const RefVar&)
MPW-mangled: SetReceiver__11TNSDebugAPIFlRC6RefVar
*/
    JT _ZN11TNSDebugAPI11SetReceiverElRK6RefVar, 01802808

/*
C++: TUSemaphoreGroup::SetRefCon(void*)
MPW-mangled: SetRefCon__16TUSemaphoreGroupFPv
*/
    JT _ZN16TUSemaphoreGroup9SetRefConEPv, 018014b4

/*
C++: TUTask::SetRegister(unsigned long, unsigned long)
MPW-mangled: SetRegister__6TUTaskFUlT1
*/
    JT SetRegister__6TUTaskFUlT1, 018014b8
    JT SetRelocHeap, 018016c0

/*
C++: TPartHandler::SetRemoveObjPtr(long)
MPW-mangled: SetRemoveObjPtr__12TPartHandlerFl
*/
    JT _ZN12TPartHandler15SetRemoveObjPtrEl, 018014bc
    JT SetRemoveRoutine, 018014c0

/*
C++: TAEventHandler::SetReply(TUMsgToken*)
MPW-mangled: SetReply__14TAEventHandlerFP10TUMsgToken
*/
    JT _ZN14TAEventHandler8SetReplyEP10TUMsgToken, 01801d84

/*
C++: TAEventHandler::SetReply(TUMsgToken*, unsigned long, TAEvent*)
MPW-mangled: SetReply__14TAEventHandlerFP10TUMsgTokenUlP7TAEvent
*/
    JT _ZN14TAEventHandler8SetReplyEP10TUMsgTokenmP7TAEvent, 01801d88

/*
C++: TAEventHandler::SetReply(unsigned long, TAEvent*)
MPW-mangled: SetReply__14TAEventHandlerFUlP7TAEvent
*/
    JT _ZN14TAEventHandler8SetReplyEmP7TAEvent, 01801d8c

/*
C++: TWRecognizer::SetScore(TWRecUnit*, unsigned long, unsigned long)
MPW-mangled: SetScore__12TWRecognizerFP9TWRecUnitUlT2
*/
    JT SetScore__12TWRecognizerFP9TWRecUnitUlT2, 01802d58

/*
C++: TInterpreter::SetSendEnv(const RefVar&, const RefVar&)
MPW-mangled: SetSendEnv__12TInterpreterFRC6RefVarT1
*/
    JT SetSendEnv__12TInterpreterFRC6RefVarT1, 01802744

/*
C++: TServiceInfo::SetServiceId(unsigned long)
MPW-mangled: SetServiceId__12TServiceInfoFUl
*/
    JT _ZN12TServiceInfo12SetServiceIdEm, 0180040c
    JT SetSkiaHeapRefcon, 018016c4
    JT SetSkiaHeapSemaphore, 018016c8

/*
C++: TCardSocket::SetSocketInterruptFlags(TSocketInt, TSocketIntFlags)
MPW-mangled: SetSocketInterruptFlags__11TCardSocketF10TSocketInt15TSocketIntFlags
*/
    JT _ZN11TCardSocket23SetSocketInterruptFlagsE10TSocketInt15TSocketIntFlags, 01802de8

/*
C++: CPartPipe::SetStreamSize(long)
MPW-mangled: SetStreamSize__9CPartPipeFl
*/
    JT _ZN9CPartPipe13SetStreamSizeEl, 018014c4

/*
C++: SetString(char*&, const char*)
MPW-mangled: SetString__FRPcPCc
*/
    JT _Z9SetStringRPcPKc, 018014c8

/*
C++: SetStringsBlock(char*&, const char*, const unsigned long)
MPW-mangled: SetStringsBlock__FRPcPCcCUl
*/
    JT _Z15SetStringsBlockRPcPKcKm, 018014cc

/*
C++: TEndpoint::SetSync(unsigned char)
MPW-mangled: SetSync__9TEndpointFUc
*/
    JT _ZN9TEndpoint7SetSyncEh, 01800410

/*
C++: TNSDebugAPI::SetTempValue(long, long, const RefVar&)
MPW-mangled: SetTempValue__11TNSDebugAPIFlT1RC6RefVar
*/
    JT SetTempValue__11TNSDebugAPIFlT1RC6RefVar, 01802830

/*
C++: static TURealTimeAlarm::SetTime(TTime)
MPW-mangled: SetTime__15TURealTimeAlarmSF5TTime
*/
    JT _ZN15TURealTimeAlarm7SetTimeE5TTime, 018014d0

/*
C++: TEndpointPipe::SetTimeout(unsigned long)
MPW-mangled: SetTimeout__13TEndpointPipeFUl
*/
    JT _ZN13TEndpointPipe10SetTimeoutEm, 01800414

/*
C++: TUSharedMemMsg::SetTimerParms(unsigned long, TTime*)
MPW-mangled: SetTimerParms__14TUSharedMemMsgFUlP5TTime
*/
    JT _ZN14TUSharedMemMsg13SetTimerParmsEmP5TTime, 018014d4

/*
C++: TAppWorld::SetTokenOnly(unsigned char)
MPW-mangled: SetTokenOnly__9TAppWorldFUc
*/
    JT _ZN9TAppWorld12SetTokenOnlyEh, 01801d90

/*
C++: TProtocol::SetType(const TClassInfo*)
MPW-mangled: SetType__9TProtocolFPC10TClassInfo
*/
    JT _ZN9TProtocol7SetTypeEPK10TClassInfo, 018014d8

/*
C++: TUSharedMemMsg::SetUserRefCon(unsigned long)
MPW-mangled: SetUserRefCon__14TUSharedMemMsgFUl
*/
    JT _ZN14TUSharedMemMsg13SetUserRefConEm, 018014dc

/*
C++: TInterpreter::SetValue(long, long)
MPW-mangled: SetValue__12TInterpreterFlT1
*/
    JT SetValue__12TInterpreterFlT1, 01801ffc

/*
C++: TView::SetValue(const RefVar&, const RefVar&)
MPW-mangled: SetValue__5TViewFRC6RefVarT1
*/
    JT SetValue__5TViewFRC6RefVarT1, 01800ce0

/*
C++: TNSDebugAPI::SetVar(long, long, const RefVar&)
MPW-mangled: SetVar__11TNSDebugAPIFlT1RC6RefVar
*/
    JT SetVar__11TNSDebugAPIFlT1RC6RefVar, 0180281c

/*
C++: SetVariableOrGlobal(const RefVar&, const RefVar&, const RefVar&, int, int)
MPW-mangled: SetVariableOrGlobal__FRC6RefVarN21iT4
*/
    JT SetVariableOrGlobal__FRC6RefVarN21iT4, 01801ee4

/*
C++: SetVariable(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: SetVariable__FRC6RefVarN21
*/
    JT SetVariable__FRC6RefVarN21, 01800a3c

/*
C++: SetVccTimeout(int, unsigned long)
MPW-mangled: SetVccTimeout__FiUl
*/
    JT _Z13SetVccTimeoutim, 018027b0

/*
C++: SetVideoCardHandler(TCardHandler*)
MPW-mangled: SetVideoCardHandler__FP12TCardHandler
*/
    JT _Z19SetVideoCardHandlerP12TCardHandler, 01801e7c

/*
C++: SetVoltageTable(VoltageTable*)
MPW-mangled: SetVoltageTable__FP12VoltageTable
*/
    JT _Z15SetVoltageTableP12VoltageTable, 01801f88

/*
C++: TCardSocket::SetWatchTimer(unsigned long)
MPW-mangled: SetWatchTimer__11TCardSocketFUl
*/
    JT _ZN11TCardSocket13SetWatchTimerEm, 018014e0
    JT SetWiredHeap, 018016cc

/*
C++: TWRecognizer::SetWordString(TWRecUnit*, unsigned long, unsigned short*)
MPW-mangled: SetWordString__12TWRecognizerFP9TWRecUnitUlPUs
*/
    JT _ZN12TWRecognizer13SetWordStringEP9TWRecUnitmPt, 01802d5c

/*
C++: TView::SetupForm()
MPW-mangled: SetupForm__5TViewFv
*/
    JT _ZN5TView9SetupFormEv, 01800cd8

/*
C++: SetupResend(const RefVar&, long, RefVar&)
MPW-mangled: SetupResend__FRC6RefVarlR6RefVar
*/
    JT _Z11SetupResendRK6RefVarlR6RefVar, 0180274c

/*
C++: SetupSend(const RefVar&, const RefVar&, long, RefVar&)
MPW-mangled: SetupSend__FRC6RefVarT1lR6RefVar
*/
    JT SetupSend__FRC6RefVarT1lR6RefVar, 01802748

/*
C++: TView::SetupVisRgn() const
MPW-mangled: SetupVisRgn__5TViewCFv
*/
    JT SetupVisRgn__5TViewCFv, 01800cdc

/*
C++: TOptionArray::ShadowCopyBack()
MPW-mangled: ShadowCopyBack__12TOptionArrayFv
*/
    JT _ZN12TOptionArray14ShadowCopyBackEv, 01800418

/*
C++: TDate::ShortDateString(unsigned long, unsigned short*, unsigned long)
MPW-mangled: ShortDateString__5TDateFUlPUsT1
*/
    JT ShortDateString__5TDateFUlPUsT1, 01802644

/*
C++: TDelayTimer::ShortTimerDelayUntil(unsigned long)
MPW-mangled: ShortTimerDelayUntil__11TDelayTimerFUl
*/
    JT _ZN11TDelayTimer20ShortTimerDelayUntilEm, 018025b0

/*
C++: TDelayTimer::ShortTimerDelay(unsigned long)
MPW-mangled: ShortTimerDelay__11TDelayTimerFUl
*/
    JT _ZN11TDelayTimer15ShortTimerDelayEm, 018025a8

/*
C++: ShortTimerDelay(unsigned long)
MPW-mangled: ShortTimerDelay__FUl
*/
    JT _Z15ShortTimerDelaym, 01802594

/*
C++: TCommTool::ShouldAbort(unsigned long, long)
MPW-mangled: ShouldAbort__9TCommToolFUll
*/
    JT _ZN9TCommTool11ShouldAbortEml, 01800758

/*
C++: TView::Show()
MPW-mangled: Show__5TViewFv
*/
    JT _ZN5TView4ShowEv, 01800ce4
    JT ShrinkHeapLeaving, 0180185c
    JT ShrinkSkiaHeapLeaving, 018016d0

/*
C++: TClassInfo::Signature() const
MPW-mangled: Signature__10TClassInfoCFv
*/
    JT Signature__10TClassInfoCFv, 018014e4

/*
C++: TView::SimpleOffset(TPoint, long)
MPW-mangled: SimpleOffset__5TViewF6TPointl
*/
    JT _ZN5TView12SimpleOffsetE6TPointl, 018027c0

/*
C++: TCompiler::Simplify(const RefVar&)
MPW-mangled: Simplify__9TCompilerFRC6RefVar
*/
    JT _ZN9TCompiler8SimplifyERK6RefVar, 01802c94

/*
C++: SizeOfPatches()
MPW-mangled: SizeOfPatches__Fv
*/
    JT _Z13SizeOfPatchesv, 018014f4

/*
C++: TClassInfo::Size() const
MPW-mangled: Size__10TClassInfoCFv
*/
    JT Size__10TClassInfoCFv, 018014e8

/*
C++: TObjectWriter::Size()
MPW-mangled: Size__13TObjectWriterFv
*/
    JT _ZN13TObjectWriter4SizeEv, 01800a40

/*
C++: TUPhys::Size(unsigned long&)
MPW-mangled: Size__6TUPhysFRUl
*/
    JT _ZN6TUPhys4SizeERm, 018014ec

/*
C++: CBufferList::Skip()
MPW-mangled: Skip__11CBufferListFv
*/
    JT _ZN11CBufferList4SkipEv, 01801d94

/*
C++: CBufferPipe::Skip()
MPW-mangled: Skip__11CBufferPipeFv
*/
    JT _ZN11CBufferPipe4SkipEv, 01801d98

/*
C++: CRingBuffer::Skip()
MPW-mangled: Skip__11CRingBufferFv
*/
    JT _ZN11CRingBuffer4SkipEv, 01801d9c

/*
C++: CBufferSegment::Skip()
MPW-mangled: Skip__14CBufferSegmentFv
*/
    JT _ZN14CBufferSegment4SkipEv, 01801da0

/*
C++: CShadowRingBuffer::Skip()
MPW-mangled: Skip__17CShadowRingBufferFv
*/
    JT _ZN17CShadowRingBuffer4SkipEv, 01801da4

/*
C++: CShadowBufferSegment::Skip()
MPW-mangled: Skip__20CShadowBufferSegmentFv
*/
    JT _ZN20CShadowBufferSegment4SkipEv, 01801da8

/*
C++: SleepTill(TTime*)
MPW-mangled: SleepTill__FP5TTime
*/
    JT _Z9SleepTillP5TTime, 018014fc

/*
C++: Sleep(unsigned long)
MPW-mangled: Sleep__FUl
*/
    JT _Z5Sleepm, 018014f8

/*
C++: Slots(long)
MPW-mangled: Slots__Fl
*/
    JT _Z5Slotsl, 01800a44

/*
C++: TEndpointClient::SndComplete(TEndpointEvent*)
MPW-mangled: SndComplete__15TEndpointClientFP14TEndpointEvent
*/
    JT _ZN15TEndpointClient11SndCompleteEP14TEndpointEvent, 01800424

/*
C++: TEndpoint::Snd(CBufferSegment*, unsigned long, unsigned long)
MPW-mangled: Snd__9TEndpointFP14CBufferSegmentUlT2
*/
    JT Snd__9TEndpointFP14CBufferSegmentUlT2, 0180041c

/*
C++: TEndpoint::Snd(unsigned char*, long&, unsigned long, unsigned long)
MPW-mangled: Snd__9TEndpointFPUcRlUlT3
*/
    JT Snd__9TEndpointFPUcRlUlT3, 01800420

/*
C++: TCardSocket::SocketBaseAddr()
MPW-mangled: SocketBaseAddr__11TCardSocketFv
*/
    JT _ZN11TCardSocket14SocketBaseAddrEv, 01802ddc

/*
C++: TCardSocket::SocketDomain()
MPW-mangled: SocketDomain__11TCardSocketFv
*/
    JT _ZN11TCardSocket12SocketDomainEv, 01801500

/*
C++: static TCardDomains::SocketDomain(unsigned long)
MPW-mangled: SocketDomain__12TCardDomainsSFUl
*/
    JT _ZN12TCardDomains12SocketDomainEm, 01801504

/*
C++: TCardSocket::SocketNumber()
MPW-mangled: SocketNumber__11TCardSocketFv
*/
    JT _ZN11TCardSocket12SocketNumberEv, 01801508

/*
C++: TCardSocket::SocketPhysResource()
MPW-mangled: SocketPhysResource__11TCardSocketFv
*/
    JT _ZN11TCardSocket18SocketPhysResourceEv, 0180150c

/*
C++: SortArray(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: SortArray__FRC6RefVarN21
*/
    JT SortArray__FRC6RefVarN21, 01800a48

/*
C++: TView::SoundEffect(const RefVar&)
MPW-mangled: SoundEffect__5TViewFRC6RefVar
*/
    JT _ZN5TView11SoundEffectERK6RefVar, 01800cec

/*
C++: SoupAddIndex(const RefVar&, const RefVar&)
MPW-mangled: SoupAddIndex__FRC6RefVarT1
*/
    JT SoupAddIndex__FRC6RefVarT1, 01800a50

/*
C++: SoupAddWithUniqueID(const RefVar&, const RefVar&)
MPW-mangled: SoupAddWithUniqueID__FRC6RefVarT1
*/
    JT SoupAddWithUniqueID__FRC6RefVarT1, 01800a54

/*
C++: SoupAdd(const RefVar&, const RefVar&)
MPW-mangled: SoupAdd__FRC6RefVarT1
*/
    JT SoupAdd__FRC6RefVarT1, 01800a4c

/*
C++: SoupCopyEntries(const RefVar&, const RefVar&)
MPW-mangled: SoupCopyEntries__FRC6RefVarT1
*/
    JT SoupCopyEntries__FRC6RefVarT1, 01800a5c

/*
C++: SoupEQ(const RefVar&, const RefVar&)
MPW-mangled: SoupEQ__FRC6RefVarT1
*/
    JT SoupEQ__FRC6RefVarT1, 01800cf0

/*
C++: SoupFlush(const RefVar&)
MPW-mangled: SoupFlush__FRC6RefVar
*/
    JT _Z9SoupFlushRK6RefVar, 01800a60

/*
C++: SoupGetAllInfo(const RefVar&)
MPW-mangled: SoupGetAllInfo__FRC6RefVar
*/
    JT _Z14SoupGetAllInfoRK6RefVar, 01800a64

/*
C++: SoupGetIndexes(const RefVar&)
MPW-mangled: SoupGetIndexes__FRC6RefVar
*/
    JT _Z14SoupGetIndexesRK6RefVar, 01800a68

/*
C++: SoupGetInfo(const RefVar&, const RefVar&)
MPW-mangled: SoupGetInfo__FRC6RefVarT1
*/
    JT SoupGetInfo__FRC6RefVarT1, 01800a6c

/*
C++: SoupGetName(const RefVar&)
MPW-mangled: SoupGetName__FRC6RefVar
*/
    JT _Z11SoupGetNameRK6RefVar, 01800a70

/*
C++: SoupGetNextUID(const RefVar&)
MPW-mangled: SoupGetNextUID__FRC6RefVar
*/
    JT _Z14SoupGetNextUIDRK6RefVar, 01800a74

/*
C++: SoupGetSignature(const RefVar&)
MPW-mangled: SoupGetSignature__FRC6RefVar
*/
    JT _Z16SoupGetSignatureRK6RefVar, 01800a78

/*
C++: SoupGetStore(const RefVar&)
MPW-mangled: SoupGetStore__FRC6RefVar
*/
    JT _Z12SoupGetStoreRK6RefVar, 01800a7c

/*
C++: SoupRemoveAllEntries(const RefVar&)
MPW-mangled: SoupRemoveAllEntries__FRC6RefVar
*/
    JT _Z20SoupRemoveAllEntriesRK6RefVar, 01800a84

/*
C++: SoupRemoveFromStore(const RefVar&)
MPW-mangled: SoupRemoveFromStore__FRC6RefVar
*/
    JT _Z19SoupRemoveFromStoreRK6RefVar, 01800a88

/*
C++: SoupRemoveIndex(const RefVar&, const RefVar&)
MPW-mangled: SoupRemoveIndex__FRC6RefVarT1
*/
    JT SoupRemoveIndex__FRC6RefVarT1, 01800a8c

/*
C++: SoupSetAllInfo(const RefVar&, const RefVar&)
MPW-mangled: SoupSetAllInfo__FRC6RefVarT1
*/
    JT SoupSetAllInfo__FRC6RefVarT1, 01800a90

/*
C++: SoupSetInfo(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: SoupSetInfo__FRC6RefVarN21
*/
    JT SoupSetInfo__FRC6RefVarN21, 01800a94

/*
C++: SoupSetName(const RefVar&, const RefVar&)
MPW-mangled: SoupSetName__FRC6RefVarT1
*/
    JT SoupSetName__FRC6RefVarT1, 01800a98

/*
C++: SoupSetSignature(const RefVar&, long)
MPW-mangled: SoupSetSignature__FRC6RefVarl
*/
    JT _Z16SoupSetSignatureRK6RefVarl, 01800a9c

/*
C++: StackTrace()
MPW-mangled: StackTrace__Fv
*/
    JT _Z10StackTracev, 01800aa0

/*
C++: TCommTool::StartAbort(long)
MPW-mangled: StartAbort__9TCommToolFl
*/
    JT _ZN9TCommTool10StartAbortEl, 0180075c

/*
C++: StartCommToolProtocol(TOptionArray*, unsigned long, TServiceInfo*, TCMService*, TCommToolProtocol*)
MPW-mangled: StartCommToolProtocol__FP12TOptionArrayUlP12TServiceInfoP10TCMServiceP17TCommToolProtocol
*/
    JT _Z21StartCommToolProtocolP12TOptionArraymP12TServiceInfoP10TCMServiceP17TCommToolProtocol, 01800764

/*
C++: StartCommToolProtocol(TOptionArray*, unsigned long, TServiceInfo*, TCMService*, TCommToolProtocol*, long)
MPW-mangled: StartCommToolProtocol__FP12TOptionArrayUlP12TServiceInfoP10TCMServiceP17TCommToolProtocoll
*/
    JT _Z21StartCommToolProtocolP12TOptionArraymP12TServiceInfoP10TCMServiceP17TCommToolProtocoll, 018026bc

/*
C++: StartCommTool(TCommTool*, unsigned long, TServiceInfo*)
MPW-mangled: StartCommTool__FP9TCommToolUlP12TServiceInfo
*/
    JT _Z13StartCommToolP9TCommToolmP12TServiceInfo, 01800760

/*
C++: TAEventHandler::StartIdle()
MPW-mangled: StartIdle__14TAEventHandlerFv
*/
    JT _ZN14TAEventHandler9StartIdleEv, 01801db0

/*
C++: TProtocol::StartMonitor(unsigned long, unsigned long, unsigned long, unsigned char)
MPW-mangled: StartMonitor__9TProtocolFUlN21Uc
*/
    JT StartMonitor__9TProtocolFUlN21Uc, 01801514

/*
C++: TUTaskWorld::StartTask(unsigned char, unsigned char, unsigned long, unsigned long, unsigned long, unsigned long)
MPW-mangled: StartTask__11TUTaskWorldFUcT1UlN33
*/
    JT StartTask__11TUTaskWorldFUcT1UlN33, 01801518

/*
C++: TUTaskWorld::StartTask(unsigned char, unsigned char, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long)
MPW-mangled: StartTask__11TUTaskWorldFUcT1UlN43
*/
    JT StartTask__11TUTaskWorldFUcT1UlN43, 0180151c

/*
C++: TCMService::Start(TOptionArray*, unsigned long, TServiceInfo*)
MPW-mangled: Start__10TCMServiceFP12TOptionArrayUlP12TServiceInfo
*/
    JT _ZN10TCMService5StartEP12TOptionArraymP12TServiceInfo, 018020bc

/*
C++: TIdleTimer::Start(TUPort*)
MPW-mangled: Start__10TIdleTimerFP6TUPort
*/
    JT _ZN10TIdleTimer5StartEP6TUPort, 01801f0c

/*
C++: TIdleTimer::Start()
MPW-mangled: Start__10TIdleTimerFv
*/
    JT _ZN10TIdleTimer5StartEv, 01801dac

/*
C++: TUSoundChannel::Start(int)
MPW-mangled: Start__14TUSoundChannelFi
*/
    JT _ZN14TUSoundChannel5StartEi, 01802c50

/*
C++: TUTask::Start()
MPW-mangled: Start__6TUTaskFv
*/
    JT _ZN6TUTask5StartEv, 01801510

/*
C++: static TUDomainManager::StaticInit(unsigned long, unsigned long)
MPW-mangled: StaticInit__15TUDomainManagerSFUlT1
*/
    JT StaticInit__15TUDomainManagerSFUlT1, 01801520

/*
C++: static TURdWrSemaphore::StaticInit()
MPW-mangled: StaticInit__15TURdWrSemaphoreSFv
*/
    JT _ZN15TURdWrSemaphore10StaticInitEv, 01801524

/*
C++: static TULockingSemaphore::StaticInit()
MPW-mangled: StaticInit__18TULockingSemaphoreSFv
*/
    JT _ZN18TULockingSemaphore10StaticInitEv, 01801528

/*
C++: Statistics(unsigned long*, unsigned long*)
MPW-mangled: Statistics__FPUlT1
*/
    JT Statistics__FPUlT1, 01800aa4

/*
C++: TAEventHandler::StopIdle()
MPW-mangled: StopIdle__14TAEventHandlerFv
*/
    JT _ZN14TAEventHandler8StopIdleEv, 01801db8

/*
C++: TIdleTimer::Stop()
MPW-mangled: Stop__10TIdleTimerFv
*/
    JT _ZN10TIdleTimer4StopEv, 01801db4

/*
C++: TUSoundChannel::Stop(SoundBlock*, long*)
MPW-mangled: Stop__14TUSoundChannelFP10SoundBlockPl
*/
    JT _ZN14TUSoundChannel4StopEP10SoundBlockPl, 01802c54
    JT StoreCreateSoup, 01800aa8

/*
C++: StoreDirty(const RefVar&)
MPW-mangled: StoreDirty__FRC6RefVar
*/
    JT _Z10StoreDirtyRK6RefVar, 01800aac
    JT StoreFlush, 01800ab0
    JT StoreGetKind, 01800ab4
    JT StoreGetName, 01800ab8
    JT StoreGetSignature, 01800abc
    JT StoreGetSoup, 01800ac0
    JT StoreGetSoupNames, 01800ac4
    JT StoreHasSoup, 01800ac8
    JT StoreIsReadOnly, 01800acc
    JT StoreLock, 01800ad0
    JT StoreOverhead, 01800ad4
    JT StoreSetName, 01800ae4

/*
C++: StoreToId(TStore*, unsigned long, unsigned long*)
MPW-mangled: StoreToId__FP6TStoreUlPUl
*/
    JT _Z9StoreToIdP6TStoremPm, 01801530
    JT StoreTotalSize, 01800ae8
    JT StoreUnlock, 01800aec
    JT StoreUsedSize, 01800af0

/*
C++: TPackageIterator::Store(TStore*, unsigned long, TCallbackCompressor*)
MPW-mangled: Store__16TPackageIteratorFP6TStoreUlP19TCallbackCompressor
*/
    JT _ZN16TPackageIterator5StoreEP6TStoremP19TCallbackCompressor, 0180152c

/*
C++: StrBeginsWith(const RefVar&, const RefVar&)
MPW-mangled: StrBeginsWith__FRC6RefVarT1
*/
    JT StrBeginsWith__FRC6RefVarT1, 01800af4

/*
C++: StrCapitalizeWords(const RefVar&)
MPW-mangled: StrCapitalizeWords__FRC6RefVar
*/
    JT _Z18StrCapitalizeWordsRK6RefVar, 01800afc

/*
C++: StrCapitalize(const RefVar&)
MPW-mangled: StrCapitalize__FRC6RefVar
*/
    JT _Z13StrCapitalizeRK6RefVar, 01800af8

/*
C++: StrDowncase(const RefVar&)
MPW-mangled: StrDowncase__FRC6RefVar
*/
    JT _Z11StrDowncaseRK6RefVar, 01800b00

/*
C++: StrEndsWith(const RefVar&, const RefVar&)
MPW-mangled: StrEndsWith__FRC6RefVarT1
*/
    JT StrEndsWith__FRC6RefVarT1, 01800b04

/*
C++: StrMunger(const RefVar&, long, long, const RefVar&, long, long)
MPW-mangled: StrMunger__FRC6RefVarlT2T1N22
*/
    JT StrMunger__FRC6RefVarlT2T1N22, 01800b08

/*
C++: StrPosition(const RefVar&, const RefVar&, long)
MPW-mangled: StrPosition__FRC6RefVarT1l
*/
    JT StrPosition__FRC6RefVarT1l, 01800b0c

/*
C++: StrReplace(const RefVar&, const RefVar&, const RefVar&, long)
MPW-mangled: StrReplace__FRC6RefVarN21l
*/
    JT StrReplace__FRC6RefVarN21l, 01800b10

/*
C++: StrUpcase(const RefVar&)
MPW-mangled: StrUpcase__FRC6RefVar
*/
    JT _Z9StrUpcaseRK6RefVar, 01800b14

/*
C++: TDate::StringToDate(const unsigned short*, unsigned long*, unsigned long)
MPW-mangled: StringToDate__5TDateFPCUsPUlUl
*/
    JT _ZN5TDate12StringToDateEPKtPmm, 01802648

/*
C++: TDate::StringToTime(const unsigned short*, unsigned long*, unsigned long)
MPW-mangled: StringToTime__5TDateFPCUsPUlUl
*/
    JT _ZN5TDate12StringToTimeEPKtPmm, 0180264c

/*
C++: Stringer(const RefVar&)
MPW-mangled: Stringer__FRC6RefVar
*/
    JT _Z8StringerRK6RefVar, 01802c98

/*
C++: StripPunctSymbols(unsigned short*)
MPW-mangled: StripPunctSymbols__FPUs
*/
    JT _Z17StripPunctSymbolsPt, 01802b70

/*
C++: TWRecognizer::StrokeSampleX(WrecSamplePt*)
MPW-mangled: StrokeSampleX__12TWRecognizerFP12WrecSamplePt
*/
    JT _ZN12TWRecognizer13StrokeSampleXEP12WrecSamplePt, 01802d60

/*
C++: TWRecognizer::StrokeSampleY(WrecSamplePt*)
MPW-mangled: StrokeSampleY__12TWRecognizerFP12WrecSamplePt
*/
    JT _ZN12TWRecognizer13StrokeSampleYEP12WrecSamplePt, 01802d64

/*
C++: TWRecognizer::StrokeSize(TStrokeUnit*)
MPW-mangled: StrokeSize__12TWRecognizerFP11TStrokeUnit
*/
    JT _ZN12TWRecognizer10StrokeSizeEP11TStrokeUnit, 01802d68

/*
C++: TWRecognizer::StrokeSize(TStroke*)
MPW-mangled: StrokeSize__12TWRecognizerFP7TStroke
*/
    JT _ZN12TWRecognizer10StrokeSizeEP7TStroke, 01802d6c

/*
C++: TWRecognizer::StrokeUnitStroke(TStrokeUnit*)
MPW-mangled: StrokeUnitStroke__12TWRecognizerFP11TStrokeUnit
*/
    JT _ZN12TWRecognizer16StrokeUnitStrokeEP11TStrokeUnit, 01802d70

/*
C++: TWRecognizer::SubCount(TWRecUnit*)
MPW-mangled: SubCount__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer8SubCountEP9TWRecUnit, 01802d74
    JT Subexception, 01801860

/*
C++: Substring(const RefVar&, long, long)
MPW-mangled: Substring__FRC6RefVarlT2
*/
    JT Substring__FRC6RefVarlT2, 01800b18

/*
C++: SuckPackageThruPipe(CPipe*, const RefVar&, const RefVar&)
MPW-mangled: SuckPackageThruPipe__FP5CPipeRC6RefVarT2
*/
    JT SuckPackageThruPipe__FP5CPipeRC6RefVarT2, 01802bbc

/*
C++: SuckPackageThruPipe(CPipe*, const RefVar&, const RefVar&, unsigned long, int)
MPW-mangled: SuckPackageThruPipe__FP5CPipeRC6RefVarT2Uli
*/
    JT SuckPackageThruPipe__FP5CPipeRC6RefVarT2Uli, 01802bc0

/*
C++: TATA::SuspendService()
MPW-mangled: SuspendService__4TATAFv
*/
    JT _ZN4TATA14SuspendServiceEv, 01802ae4

/*
C++: TUTask::Suspend()
MPW-mangled: Suspend__6TUTaskFv
*/
    JT _ZN6TUTask7SuspendEv, 01801534

/*
C++: TCardCISIterator::SwapLittleEndianLong(unsigned char*)
MPW-mangled: SwapLittleEndianLong__16TCardCISIteratorFPUc
*/
    JT _ZN16TCardCISIterator20SwapLittleEndianLongEPh, 01802500

/*
C++: TCardCISIterator::SwapLittleEndianShort(unsigned char*)
MPW-mangled: SwapLittleEndianShort__16TCardCISIteratorFPUc
*/
    JT _ZN16TCardCISIterator21SwapLittleEndianShortEPh, 0180250c

/*
C++: CArrayIterator::SwitchArray(CDynamicArray*, unsigned char)
MPW-mangled: SwitchArray__14CArrayIteratorFP13CDynamicArrayUc
*/
    JT _ZN14CArrayIterator11SwitchArrayEP13CDynamicArrayh, 01801dbc

/*
C++: SymbolCompareLex(const RefVar&, const RefVar&)
MPW-mangled: SymbolCompareLex__FRC6RefVarT1
*/
    JT SymbolCompareLex__FRC6RefVarT1, 018029e8

/*
C++: SymbolCompare(long, long)
MPW-mangled: SymbolCompare__FlT1
*/
    JT SymbolCompare__FlT1, 01800b1c

/*
C++: SymbolHash(long)
MPW-mangled: SymbolHash__Fl
*/
    JT _Z10SymbolHashl, 01800b20

/*
C++: SymbolName(long)
MPW-mangled: SymbolName__Fl
*/
    JT _Z10SymbolNamel, 01800b24

/*
C++: TView::SyncScroll(const RefVar&, const RefVar&, const RefVar&)
MPW-mangled: SyncScroll__5TViewFRC6RefVarN21
*/
    JT SyncScroll__5TViewFRC6RefVarN21, 01800cf8

/*
C++: TView::Sync()
MPW-mangled: Sync__5TViewFv
*/
    JT _ZN5TView4SyncEv, 01800cf4
    JT SystemFreePageCount, 01801538
    JT SystemRAMSize, 01801864

/*
C++: TBCInsertTabletSample(unsigned long, unsigned long)
MPW-mangled: TBCInsertTabletSample__FUlT1
*/
    JT _Z21TBCInsertTabletSamplemm, 018027d0

/*
C++: TBCWakeUpInkerFromInterrupt(unsigned long)
MPW-mangled: TBCWakeUpInkerFromInterrupt__FUl
*/
    JT _Z27TBCWakeUpInkerFromInterruptm, 018027d4

/*
C++: TBCWakeUpInker(unsigned long)
MPW-mangled: TBCWakeUpInker__FUl
*/
    JT _Z14TBCWakeUpInkerm, 018027d8

/*
C++: TagStringHelper(const RefVar&, RefVar)
MPW-mangled: TagStringHelper__FRC6RefVar6RefVar
*/
    JT _Z15TagStringHelperRK6RefVar6RefVar, 01802774

/*
C++: TObjectIterator::Tag()
MPW-mangled: Tag__15TObjectIteratorFv
*/
    JT _ZN15TObjectIterator3TagEv, 01802c9c
    JT TapFileCntl, 0180002c

/*
C++: TaskAcceptObject(unsigned long)
MPW-mangled: TaskAcceptObject__FUl
*/
    JT _Z16TaskAcceptObjectm, 0180153c

/*
C++: TUTaskWorld::TaskConstructor()
MPW-mangled: TaskConstructor__11TUTaskWorldFv
*/
    JT _ZN11TUTaskWorld15TaskConstructorEv, 01801540

/*
C++: TCommTool::TaskConstructor()
MPW-mangled: TaskConstructor__9TCommToolFv
*/
    JT _ZN9TCommTool15TaskConstructorEv, 01800768

/*
C++: TUTaskWorld::TaskDestructor()
MPW-mangled: TaskDestructor__11TUTaskWorldFv
*/
    JT _ZN11TUTaskWorld14TaskDestructorEv, 01801544

/*
C++: TCommTool::TaskDestructor()
MPW-mangled: TaskDestructor__9TCommToolFv
*/
    JT _ZN9TCommTool14TaskDestructorEv, 0180076c

/*
C++: TUTaskWorld::TaskEntry(unsigned long, unsigned long)
MPW-mangled: TaskEntry__11TUTaskWorldFUlT1
*/
    JT TaskEntry__11TUTaskWorldFUlT1, 01801548

/*
C++: TaskGiveObject(unsigned long, unsigned long)
MPW-mangled: TaskGiveObject__FUlT1
*/
    JT TaskGiveObject__FUlT1, 0180154c
    JT TaskKillSelf, 01801550

/*
C++: TCommTool::TaskMain()
MPW-mangled: TaskMain__9TCommToolFv
*/
    JT _ZN9TCommTool8TaskMainEv, 01800770

/*
C++: TEzEndpointPipe::TearDown()
MPW-mangled: TearDown__15TEzEndpointPipeFv
*/
    JT _ZN15TEzEndpointPipe8TearDownEv, 01800428

/*
C++: CShadowRingBuffer::TempCopyOut(unsigned char*, long&)
MPW-mangled: TempCopyOut__17CShadowRingBufferFPUcRl
*/
    JT _ZN17CShadowRingBuffer11TempCopyOutEPhRl, 01801dcc

/*
C++: CShadowRingBuffer::TempDataCount() const
MPW-mangled: TempDataCount__17CShadowRingBufferCFv
*/
    JT TempDataCount__17CShadowRingBufferCFv, 01801dd0

/*
C++: CShadowRingBuffer::TempGetn(unsigned char*, long)
MPW-mangled: TempGetn__17CShadowRingBufferFPUcl
*/
    JT _ZN17CShadowRingBuffer8TempGetnEPhl, 01801dd4

/*
C++: CShadowRingBuffer::TempReset()
MPW-mangled: TempReset__17CShadowRingBufferFv
*/
    JT _ZN17CShadowRingBuffer9TempResetEv, 01801dd8

/*
C++: TNSDebugAPI::TempValue(long, long)
MPW-mangled: TempValue__11TNSDebugAPIFlT1
*/
    JT TempValue__11TNSDebugAPIFlT1, 0180282c

/*
C++: TCommTool::TerminateComplete()
MPW-mangled: TerminateComplete__9TCommToolFv
*/
    JT _ZN9TCommTool17TerminateCompleteEv, 01800774

/*
C++: TCommTool::TerminateConnection()
MPW-mangled: TerminateConnection__9TCommToolFv
*/
    JT _ZN9TCommTool19TerminateConnectionEv, 01800778

/*
C++: TEndpointEventHandler::TerminateEventLoop()
MPW-mangled: TerminateEventLoop__21TEndpointEventHandlerFv
*/
    JT _ZN21TEndpointEventHandler18TerminateEventLoopEv, 01801f04

/*
C++: TAppWorldState::TerminateNestedEventLoop()
MPW-mangled: TerminateNestedEventLoop__14TAppWorldStateFv
*/
    JT _ZN14TAppWorldState24TerminateNestedEventLoopEv, 01801efc

/*
C++: TWRecognizer::TestClassifiedUnit(TWRecUnit*)
MPW-mangled: TestClassifiedUnit__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer18TestClassifiedUnitEP9TWRecUnit, 01802d78

/*
C++: TTestReporter::TestFPrintf(int, const char*, ...)
MPW-mangled: TestFPrintf__13TTestReporterFiPCce
*/
    JT _ZN13TTestReporter11TestFPrintfEiPKcz, 018020c8

/*
C++: TTestReporter::TestFlushReportQueue()
MPW-mangled: TestFlushReportQueue__13TTestReporterFv
*/
    JT _ZN13TTestReporter20TestFlushReportQueueEv, 018020c4

/*
C++: TWRecognizer::TestInvalidUnit(TWRecUnit*)
MPW-mangled: TestInvalidUnit__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer15TestInvalidUnitEP9TWRecUnit, 01802d7c

/*
C++: CItemTester::TestItem(const void*) const
MPW-mangled: TestItem__11CItemTesterCFPCv
*/
    JT TestItem__11CItemTesterCFPCv, 01801ddc

/*
C++: CItemComparer::TestItem(const void*) const
MPW-mangled: TestItem__13CItemComparerCFPCv
*/
    JT TestItem__13CItemComparerCFPCv, 01801de0

/*
C++: TAEHandlerComparer::TestItem(const void*) const
MPW-mangled: TestItem__18TAEHandlerComparerCFPCv
*/
    JT TestItem__18TAEHandlerComparerCFPCv, 01801de4

/*
C++: SysEventItemComparer::TestItem(const void*) const
MPW-mangled: TestItem__20SysEventItemComparerCFPCv
*/
    JT TestItem__20SysEventItemComparerCFPCv, 01801554

/*
C++: TTestReporter::TestReadDataFile(char*, long, long*, char**)
MPW-mangled: TestReadDataFile__13TTestReporterFPclPlPPc
*/
    JT _ZN13TTestReporter16TestReadDataFileEPclPlPPc, 018020cc

/*
C++: TWRecognizer::TestRejectedUnit(TWRecUnit*)
MPW-mangled: TestRejectedUnit__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer16TestRejectedUnitEP9TWRecUnit, 01802d80

/*
C++: TTestReporter::TestReportErrorValues(char*, char*, long, long)
MPW-mangled: TestReportErrorValues__13TTestReporterFPcT1lT3
*/
    JT TestReportErrorValues__13TTestReporterFPcT1lT3, 018020d4

/*
C++: TTestReporter::TestReportError(char*, char*, long)
MPW-mangled: TestReportError__13TTestReporterFPcT1l
*/
    JT TestReportError__13TTestReporterFPcT1l, 018020d0

/*
C++: TTestReporter::TestReportMessage(char*, char*)
MPW-mangled: TestReportMessage__13TTestReporterFPcT1
*/
    JT TestReportMessage__13TTestReporterFPcT1, 018020d8
    JT Throw, 01801868

/*
C++: ThrowBadTypeWithFrameData(long, const RefVar&)
MPW-mangled: ThrowBadTypeWithFrameData__FlRC6RefVar
*/
    JT _Z25ThrowBadTypeWithFrameDatalRK6RefVar, 01802b28
    JT ThrowMsg, 0180186c

/*
C++: ThrowRefException(char*, const RefVar&)
MPW-mangled: ThrowRefException__FPcRC6RefVar
*/
    JT _Z17ThrowRefExceptionPcRK6RefVar, 01800b28

/*
C++: Ticks()
MPW-mangled: Ticks__Fv
*/
    JT _Z5Ticksv, 01801fec
    JT TimeFromNow, 0180155c

/*
C++: TDate::TimeString(unsigned long, unsigned short*, unsigned long)
MPW-mangled: TimeString__5TDateFUlPUsT1
*/
    JT TimeString__5TDateFUlPUsT1, 01802650

/*
C++: static TURealTimeAlarm::Time()
MPW-mangled: Time__15TURealTimeAlarmSFv
*/
    JT _ZN15TURealTimeAlarm4TimeEv, 01801558

/*
C++: TDelayTimer::TimedOut(unsigned long)
MPW-mangled: TimedOut__11TDelayTimerFUl
*/
    JT _ZN11TDelayTimer8TimedOutEm, 018025b8

/*
C++: TDelayTimer::TimedOut()
MPW-mangled: TimedOut__11TDelayTimerFv
*/
    JT _ZN11TDelayTimer8TimedOutEv, 018025b4

/*
C++: ToObject(TStore*)
MPW-mangled: ToObject__FP6TStore
*/
    JT _Z8ToObjectP6TStore, 01802020

/*
C++: ToObject(const TDate&)
MPW-mangled: ToObject__FRC5TDate
*/
    JT _Z8ToObjectRK5TDate, 01802654

/*
C++: ToObject(const TRect&)
MPW-mangled: ToObject__FRC5TRect
*/
    JT _Z8ToObjectRK5TRect, 01801e68

/*
C++: ToggleCase(unsigned short)
MPW-mangled: ToggleCase__FUs
*/
    JT _Z10ToggleCaset, 01801de8

/*
C++: TAppWorld::TokenOnly()
MPW-mangled: TokenOnly__9TAppWorldFv
*/
    JT _ZN9TAppWorld9TokenOnlyEv, 01801dec

/*
C++: TotalClone(const RefVar&)
MPW-mangled: TotalClone__FRC6RefVar
*/
    JT _Z10TotalCloneRK6RefVar, 01800b2c

/*
C++: TDate::TotalDays() const
MPW-mangled: TotalDays__5TDateCFv
*/
    JT TotalDays__5TDateCFv, 01802658
    JT TotalFreeInHeap, 018016d4

/*
C++: TDate::TotalHours() const
MPW-mangled: TotalHours__5TDateCFv
*/
    JT TotalHours__5TDateCFv, 01802668

/*
C++: TDate::TotalMinutes() const
MPW-mangled: TotalMinutes__5TDateCFv
*/
    JT TotalMinutes__5TDateCFv, 0180265c

/*
C++: TDate::TotalSeconds() const
MPW-mangled: TotalSeconds__5TDateCFv
*/
    JT TotalSeconds__5TDateCFv, 0180266c

/*
C++: TSoupIndex::TotalSize()
MPW-mangled: TotalSize__10TSoupIndexFv
*/
    JT _ZN10TSoupIndex9TotalSizeEv, 01800b30

/*
C++: TStoreHashTable::TotalSize()
MPW-mangled: TotalSize__15TStoreHashTableFv
*/
    JT _ZN15TStoreHashTable9TotalSizeEv, 01800b34
    JT TotalSystemFree, 01801870
    JT TotalUsedInHeap, 018016d8

/*
C++: TView::TransferCopyProtection(const RefVar&)
MPW-mangled: TransferCopyProtection__5TViewFRC6RefVar
*/
    JT _ZN5TView22TransferCopyProtectionERK6RefVar, 01800d04

/*
C++: TInterpreter::TranslateException(Exception*)
MPW-mangled: TranslateException__12TInterpreterFP9Exception
*/
    JT _ZN12TInterpreter18TranslateExceptionEP9Exception, 018020dc

/*
C++: TrimString(const RefVar&)
MPW-mangled: TrimString__FRC6RefVar
*/
    JT _Z10TrimStringRK6RefVar, 01800b38

/*
C++: UMinMax_noninline(unsigned long, unsigned long, unsigned long)
MPW-mangled: UMinMax_noninline__FUlN21
*/
    JT UMinMax_noninline__FUlN21, 01801df4
    JT Umbstrlen, 01802b58
    JT Umbstrnlen, 01802b5c
    JT Umemset, 01801df0

/*
C++: TEndpoint::UnBind(unsigned long)
MPW-mangled: UnBind__9TEndpointFUl
*/
    JT _ZN9TEndpoint6UnBindEm, 0180042c

/*
C++: UnRegTaskTemplate(const RefVar&, const RefVar&)
MPW-mangled: UnRegTaskTemplate__FRC6RefVarT1
*/
    JT UnRegTaskTemplate__FRC6RefVarT1, 01800788

/*
C++: TSystemEvent::UnRegisterForSystemEvent(unsigned long)
MPW-mangled: UnRegisterForSystemEvent__12TSystemEventFUl
*/
    JT _ZN12TSystemEvent24UnRegisterForSystemEventEm, 01801578

/*
C++: TUNameServer::UnRegisterName(char*, char*)
MPW-mangled: UnRegisterName__12TUNameServerFPcT1
*/
    JT _ZN12TUNameServer14UnRegisterNameEPcS0_, 0180157c
    JT _ZN12TUNameServer14UnRegisterNameEPKcS1_, 0180157c /* const char* variant */

/*
C++: TCommTool::UnRegisterPort()
MPW-mangled: UnRegisterPort__9TCommToolFv
*/
    JT _ZN9TCommTool14UnRegisterPortEv, 01800780

/*
C++: CRingBuffer::UnShare()
MPW-mangled: UnShare__11CRingBufferFv
*/
    JT _ZN11CRingBuffer7UnShareEv, 01801dfc

/*
C++: TOptionArray::UnShare()
MPW-mangled: UnShare__12TOptionArrayFv
*/
    JT _ZN12TOptionArray7UnShareEv, 01800434

/*
C++: CBufferSegment::UnShare()
MPW-mangled: UnShare__14CBufferSegmentFv
*/
    JT _ZN14CBufferSegment7UnShareEv, 01801e00

/*
C++: TCommTool::Unbind()
MPW-mangled: Unbind__9TCommToolFv
*/
    JT _ZN9TCommTool6UnbindEv, 0180077c

/*
C++: TEndpointPipe::Underflow(long, unsigned char&)
MPW-mangled: Underflow__13TEndpointPipeFlRUc
*/
    JT _ZN13TEndpointPipe9UnderflowElRh, 01800430

/*
C++: CNullPipe::Underflow(long, unsigned char&)
MPW-mangled: Underflow__9CNullPipeFlRUc
*/
    JT _ZN9CNullPipe9UnderflowElRh, 01801df8

/*
C++: CPartPipe::Underflow(long, unsigned char&)
MPW-mangled: Underflow__9CPartPipeFlRUc
*/
    JT _ZN9CPartPipe9UnderflowElRh, 01801560

/*
C++: CTaskPipe::Underflow(long, unsigned char&)
MPW-mangled: Underflow__9CTaskPipeFlRUc
*/
    JT _ZN9CTaskPipe9UnderflowElRh, 01801564

/*
C++: TUSoundChannel::UniqueId()
MPW-mangled: UniqueId__14TUSoundChannelFv
*/
    JT _ZN14TUSoundChannel8UniqueIdEv, 01802c58

/*
C++: TWRecognizer::UnitInfoGetPtr(TWRecUnit*)
MPW-mangled: UnitInfoGetPtr__12TWRecognizerFP9TWRecUnit
*/
    JT _ZN12TWRecognizer14UnitInfoGetPtrEP9TWRecUnit, 01802d84

/*
C++: TWRecognizer::UnitInfoSetPtr(TWRecUnit*, char*)
MPW-mangled: UnitInfoSetPtr__12TWRecognizerFP9TWRecUnitPc
*/
    JT _ZN12TWRecognizer14UnitInfoSetPtrEP9TWRecUnitPc, 01802d88
    JT UnlockHeapRange, 0180156c
    JT UnlockPtr, 01801874

/*
C++: UnlockRefArg(const RefVar&)
MPW-mangled: UnlockRefArg__FRC6RefVar
*/
    JT _Z12UnlockRefArgRK6RefVar, 01802b54

/*
C++: UnlockRef(long)
MPW-mangled: UnlockRef__Fl
*/
    JT _Z9UnlockRefl, 01800b3c
    JT UnlockStack, 01801570

/*
C++: TLicenseeVAddress::Unmap()
MPW-mangled: Unmap__17TLicenseeVAddressFv
*/
    JT _ZN17TLicenseeVAddress5UnmapEv, 01802ae8
    JT UnmaskInterrupt, 01802bf0

/*
C++: TPartHandler::Unregister()
MPW-mangled: Unregister__12TPartHandlerFv
*/
    JT _ZN12TPartHandler10UnregisterEv, 01801574

/*
C++: CRingBuffer::UpdateGetVector(long)
MPW-mangled: UpdateGetVector__11CRingBufferFl
*/
    JT _ZN11CRingBuffer15UpdateGetVectorEl, 01801e04

/*
C++: CShadowRingBuffer::UpdateGetVector(long)
MPW-mangled: UpdateGetVector__17CShadowRingBufferFl
*/
    JT _ZN17CShadowRingBuffer15UpdateGetVectorEl, 01801e08

/*
C++: CRingBuffer::UpdatePutVector(long)
MPW-mangled: UpdatePutVector__11CRingBufferFl
*/
    JT _ZN11CRingBuffer15UpdatePutVectorEl, 01801e0c

/*
C++: CShadowRingBuffer::UpdatePutVector(long)
MPW-mangled: UpdatePutVector__17CShadowRingBufferFl
*/
    JT _ZN17CShadowRingBuffer15UpdatePutVectorEl, 01801e10

/*
C++: TView::Update(TBaseRegion, TView*)
MPW-mangled: Update__5TViewF11TBaseRegionP5TView
*/
    JT _ZN5TView6UpdateE11TBaseRegionP5TView, 01800d08

/*
C++: UppercaseNoDiacriticsText(unsigned short*, long)
MPW-mangled: UppercaseNoDiacriticsText__FPUsl
*/
    JT _Z25UppercaseNoDiacriticsTextPtl, 01801e14

/*
C++: UppercaseText(unsigned short*, long)
MPW-mangled: UppercaseText__FPUsl
*/
    JT _Z13UppercaseTextPtl, 01801e18

/*
C++: UriahBinaryObjects(int)
MPW-mangled: UriahBinaryObjects__Fi
*/
    JT _Z18UriahBinaryObjectsi, 01800b44

/*
C++: Uriah()
MPW-mangled: Uriah__Fv
*/
    JT _Z5Uriahv, 01800b40

/*
C++: TEndpointPipe::UseFraming(unsigned char)
MPW-mangled: UseFraming__13TEndpointPipeFUc
*/
    JT _ZN13TEndpointPipe10UseFramingEh, 01800438

/*
C++: UseKMEMode()
MPW-mangled: UseKMEMode__Fv
*/
    JT _Z10UseKMEModev, 018027a0

/*
C++: TROMDomainManager1K::UserRequest(long, void*)
MPW-mangled: UserRequest__19TROMDomainManager1KFlPv
*/
    JT _ZN19TROMDomainManager1K11UserRequestElPv, 01801580

/*
C++: TEndpointPipe::UsingFraming()
MPW-mangled: UsingFraming__13TEndpointPipeFv
*/
    JT _ZN13TEndpointPipe12UsingFramingEv, 0180043c
    JT Ustrcat, 01801e1c
    JT Ustrchr, 01801e20
    JT Ustrcmp, 01801e24
    JT Ustrcpy, 01801e28
    JT Ustrlen, 01801e2c
    JT Ustrncat, 01801e30
    JT Ustrncpy, 01801e34

/*
C++: VToSizeWithP(unsigned long, unsigned long&)
MPW-mangled: VToSizeWithP__FUlRUl
*/
    JT _Z12VToSizeWithPmRm, 018015f4

/*
C++: VToUnit(unsigned long)
MPW-mangled: VToUnit__FUl
*/
    JT _Z7VToUnitm, 018015f8
    JT ValidateHeap, 018016dc

/*
C++: TInterpreter::ValuePosition()
MPW-mangled: ValuePosition__12TInterpreterFv
*/
    JT _ZN12TInterpreter13ValuePositionEv, 01802000

/*
C++: TObjectIterator::Value()
MPW-mangled: Value__15TObjectIteratorFv
*/
    JT _ZN15TObjectIterator5ValueEv, 01802ca0

/*
C++: VccIdleOff(unsigned char)
MPW-mangled: VccIdleOff__FUc
*/
    JT _Z10VccIdleOffh, 01801584

/*
C++: TCardSocket::VccMaxCurrent()
MPW-mangled: VccMaxCurrent__11TCardSocketFv
*/
    JT _ZN11TCardSocket13VccMaxCurrentEv, 01801588

/*
C++: TCardSocket::VccMaxVoltage()
MPW-mangled: VccMaxVoltage__11TCardSocketFv
*/
    JT _ZN11TCardSocket13VccMaxVoltageEv, 0180158c

/*
C++: TCardSocket::VccMinVoltage()
MPW-mangled: VccMinVoltage__11TCardSocketFv
*/
    JT _ZN11TCardSocket13VccMinVoltageEv, 01801590

/*
C++: TCardSocket::VccOff()
MPW-mangled: VccOff__11TCardSocketFv
*/
    JT _ZN11TCardSocket6VccOffEv, 01801594

/*
C++: VccOff(int)
MPW-mangled: VccOff__Fi
*/
    JT _Z6VccOffi, 01801598

/*
C++: VccOff(int, unsigned long)
MPW-mangled: VccOff__FiUl
*/
    JT _Z6VccOffim, 0180159c

/*
C++: TCardSocket::VccOn()
MPW-mangled: VccOn__11TCardSocketFv
*/
    JT _ZN11TCardSocket5VccOnEv, 018015a0

/*
C++: VccOn(int, unsigned char)
MPW-mangled: VccOn__FiUc
*/
    JT _Z5VccOnih, 018015a4

/*
C++: TCardSocket::VccRisingTime()
MPW-mangled: VccRisingTime__11TCardSocketFv
*/
    JT _ZN11TCardSocket13VccRisingTimeEv, 018015a8

/*
C++: TCardSocket::VccVoltageSpec()
MPW-mangled: VccVoltageSpec__11TCardSocketFv
*/
    JT _ZN11TCardSocket14VccVoltageSpecEv, 01802aec

/*
C++: TCardCISIterator::VerifyLinkTargetTuple(unsigned char*, unsigned char*, unsigned char)
MPW-mangled: VerifyLinkTargetTuple__16TCardCISIteratorFPUcT1Uc
*/
    JT VerifyLinkTargetTuple__16TCardCISIteratorFPUcT1Uc, 01802da4

/*
C++: TPackageIterator::VerifyPackage()
MPW-mangled: VerifyPackage__16TPackageIteratorFv
*/
    JT _ZN16TPackageIterator13VerifyPackageEv, 018015ac

/*
C++: TClassInfo::Version() const
MPW-mangled: Version__10TClassInfoCFv
*/
    JT Version__10TClassInfoCFv, 018015b0

/*
C++: TCardPCMCIA::Version()
MPW-mangled: Version__11TCardPCMCIAFv
*/
    JT _ZN11TCardPCMCIA7VersionEv, 01802af0

/*
C++: TCardSocket::Version()
MPW-mangled: Version__11TCardSocketFv
*/
    JT _ZN11TCardSocket7VersionEv, 01802af4

/*
C++: TPCMCIA20Parser::Version()
MPW-mangled: Version__15TPCMCIA20ParserFv
*/
    JT _ZN15TPCMCIA20Parser7VersionEv, 01802588

/*
C++: TCardCISIterator::Version()
MPW-mangled: Version__16TCardCISIteratorFv
*/
    JT _ZN16TCardCISIterator7VersionEv, 01802af8
    JT VetHeap, 018016e0

/*
C++: TView::ViewVisibleChanged(TView*, unsigned char)
MPW-mangled: ViewVisibleChanged__5TViewFP5TViewUc
*/
    JT _ZN5TView18ViewVisibleChangedEP5TViewh, 01800d0c

/*
C++: TView::VisibleDeep() const
MPW-mangled: VisibleDeep__5TViewCFv
*/
    JT VisibleDeep__5TViewCFv, 01800d10
    JT VoidStarToHeap, 01801878

/*
C++: TCardSocket::Vpp1MaxCurrent()
MPW-mangled: Vpp1MaxCurrent__11TCardSocketFv
*/
    JT _ZN11TCardSocket14Vpp1MaxCurrentEv, 018015b4

/*
C++: TCardSocket::Vpp1Off()
MPW-mangled: Vpp1Off__11TCardSocketFv
*/
    JT _ZN11TCardSocket7Vpp1OffEv, 018015b8

/*
C++: TCardSocket::Vpp1On()
MPW-mangled: Vpp1On__11TCardSocketFv
*/
    JT _ZN11TCardSocket6Vpp1OnEv, 018015bc

/*
C++: TCardSocket::Vpp2MaxCurrent()
MPW-mangled: Vpp2MaxCurrent__11TCardSocketFv
*/
    JT _ZN11TCardSocket14Vpp2MaxCurrentEv, 018015c0

/*
C++: TCardSocket::Vpp2Off()
MPW-mangled: Vpp2Off__11TCardSocketFv
*/
    JT _ZN11TCardSocket7Vpp2OffEv, 018015c4

/*
C++: TCardSocket::Vpp2On()
MPW-mangled: Vpp2On__11TCardSocketFv
*/
    JT _ZN11TCardSocket6Vpp2OnEv, 018015c8

/*
C++: VppIdleOff(unsigned char)
MPW-mangled: VppIdleOff__FUc
*/
    JT _Z10VppIdleOffh, 018015cc

/*
C++: TCardSocket::VppMaxCurrent()
MPW-mangled: VppMaxCurrent__11TCardSocketFv
*/
    JT _ZN11TCardSocket13VppMaxCurrentEv, 018015d0

/*
C++: TCardSocket::VppMaxVoltage()
MPW-mangled: VppMaxVoltage__11TCardSocketFv
*/
    JT _ZN11TCardSocket13VppMaxVoltageEv, 018015d4

/*
C++: TCardSocket::VppMinVoltage()
MPW-mangled: VppMinVoltage__11TCardSocketFv
*/
    JT _ZN11TCardSocket13VppMinVoltageEv, 018015d8

/*
C++: TCardSocket::VppOff()
MPW-mangled: VppOff__11TCardSocketFv
*/
    JT _ZN11TCardSocket6VppOffEv, 018015dc

/*
C++: VppOff(int)
MPW-mangled: VppOff__Fi
*/
    JT _Z6VppOffi, 018015e0

/*
C++: VppOff(int, unsigned long)
MPW-mangled: VppOff__FiUl
*/
    JT _Z6VppOffim, 018015e4

/*
C++: TCardSocket::VppOn()
MPW-mangled: VppOn__11TCardSocketFv
*/
    JT _ZN11TCardSocket5VppOnEv, 018015e8

/*
C++: VppOn(int, unsigned char)
MPW-mangled: VppOn__FiUc
*/
    JT _Z5VppOnih, 018015ec

/*
C++: TCardSocket::VppRisingTime()
MPW-mangled: VppRisingTime__11TCardSocketFv
*/
    JT _ZN11TCardSocket13VppRisingTimeEv, 018015f0

/*
C++: TCardSocket::VppVoltageSpec()
MPW-mangled: VppVoltageSpec__11TCardSocketFv
*/
    JT _ZN11TCardSocket14VppVoltageSpecEv, 01802afc

/*
C++: TEndpoint::WaitForEvent(unsigned long)
MPW-mangled: WaitForEvent__9TEndpointFUl
*/
    JT _ZN9TEndpoint12WaitForEventEm, 01800440

/*
C++: TUNameServer::WaitForRegister(char*, char*, unsigned long*, unsigned long*)
MPW-mangled: WaitForRegister__12TUNameServerFPcT1PUlT3
*/
    JT WaitForRegister__12TUNameServerFPcT1PUlT3, 018015fc

/*
C++: TUNameServer::WaitForUnregister(char*, char*)
MPW-mangled: WaitForUnregister__12TUNameServerFPcT1
*/
    JT WaitForUnregister__12TUNameServerFPcT1, 01801600

/*
C++: TView::WriteBounds(const TRect&)
MPW-mangled: WriteBounds__5TViewFRC5TRect
*/
    JT _ZN5TView11WriteBoundsERK5TRect, 01800d14

/*
C++: CBufferPipe::WriteChunk(void*, long, unsigned char)
MPW-mangled: WriteChunk__11CBufferPipeFPvlUc
*/
    JT _ZN11CBufferPipe10WriteChunkEPvlh, 01801e38

/*
C++: CPartPipe::WriteChunk(void*, long, unsigned char)
MPW-mangled: WriteChunk__9CPartPipeFPvlUc
*/
    JT _ZN9CPartPipe10WriteChunkEPvlh, 01801604

/*
C++: CRingPipe::WriteChunk(void*, long, unsigned char)
MPW-mangled: WriteChunk__9CRingPipeFPvlUc
*/
    JT _ZN9CRingPipe10WriteChunkEPvlh, 01801e3c

/*
C++: WriteFaultBlock(const RefVar&)
MPW-mangled: WriteFaultBlock__FRC6RefVar
*/
    JT _Z15WriteFaultBlockRK6RefVar, 01800b4c

/*
C++: TGPIOInterface::WriteGPIOData(unsigned char, unsigned char, unsigned char*)
MPW-mangled: WriteGPIOData__14TGPIOInterfaceFUcT1PUc
*/
    JT WriteGPIOData__14TGPIOInterfaceFUcT1PUc, 01802cdc

/*
C++: CBufferPipe::WritePosition() const
MPW-mangled: WritePosition__11CBufferPipeCFv
*/
    JT WritePosition__11CBufferPipeCFv, 01801e40

/*
C++: CPartPipe::WritePosition() const
MPW-mangled: WritePosition__9CPartPipeCFv
*/
    JT WritePosition__9CPartPipeCFv, 01801608

/*
C++: CRingPipe::WritePosition() const
MPW-mangled: WritePosition__9CRingPipeCFv
*/
    JT WritePosition__9CRingPipeCFv, 01801e44

/*
C++: CBufferPipe::WriteSeek(long, int)
MPW-mangled: WriteSeek__11CBufferPipeFli
*/
    JT _ZN11CBufferPipe9WriteSeekEli, 01801e48

/*
C++: CPartPipe::WriteSeek(long, int)
MPW-mangled: WriteSeek__9CPartPipeFli
*/
    JT _ZN9CPartPipe9WriteSeekEli, 0180160c

/*
C++: CRingPipe::WriteSeek(long, int)
MPW-mangled: WriteSeek__9CRingPipeFli
*/
    JT _ZN9CRingPipe9WriteSeekEli, 01801e4c

/*
C++: TObjectWriter::Write()
MPW-mangled: Write__13TObjectWriterFv
*/
    JT _ZN13TObjectWriter5WriteEv, 01800b48

/*
C++: TATA::Write(unsigned char*, unsigned long, unsigned long, unsigned char, unsigned char)
MPW-mangled: Write__4TATAFPUcUlT2UcT4
*/
    JT Write__4TATAFPUcUlT2UcT4, 01802b00
    JT XORBytes, 018016e4

/*
C++: Yield(unsigned long)
MPW-mangled: Yield__FUl
*/
    JT _Z5Yieldm, 01801610
    JT ZapHeap, 0180187c
    JT ZeroBytes, 018016e8
    JT _ANSI_rand, 018028a4
    JT _ANSI_srand, 018028a8

/*
C++: _RCHARError(long)
MPW-mangled: _RCHARError__Fl
*/
    JT _Z11_RCHARErrorl, 018007e4

/*
C++: _RINTError(long)
MPW-mangled: _RINTError__Fl
*/
    JT _Z10_RINTErrorl, 018007e8

/*
C++: _RPTRError(long)
MPW-mangled: _RPTRError__Fl
*/
    JT _Z10_RPTRErrorl, 018007ec

/*
C++: __GetErrNo()
MPW-mangled: __GetErrNo__Fv
*/
    JT __GetErrNo__Fv, 01800e00

/*
C++: TObjectPtr::operator=(const TObjectPtr&)
MPW-mangled: __as__10TObjectPtrFRC10TObjectPtr
*/
    JT _ZN10TObjectPtraSERK10TObjectPtr, 01800790

/*
C++: TObjectPtr::operator=(long)
MPW-mangled: __as__10TObjectPtrFl
*/
    JT _ZN10TObjectPtraSEl, 0180078c

/*
C++: TUAsyncMessage::operator=(const TUMsgToken&)
MPW-mangled: __as__14TUAsyncMessageFRC10TUMsgToken
*/
    JT _ZN14TUAsyncMessageaSERK10TUMsgToken, 01800d18

/*
C++: TLicenseeVAddress::operator=(const TLicenseeVAddress&)
MPW-mangled: __as__17TLicenseeVAddressFRC17TLicenseeVAddress
*/
    JT _ZN17TLicenseeVAddressaSERK17TLicenseeVAddress, 01802560

/*
C++: TLicenseeVAddress::operator=(const TUPhys&)
MPW-mangled: __as__17TLicenseeVAddressFRC6TUPhys
*/
    JT _ZN17TLicenseeVAddressaSERK6TUPhys, 0180255c

/*
C++: TCardConfiguration::operator=(const TCardConfiguration&)
MPW-mangled: __as__18TCardConfigurationFRC18TCardConfiguration
*/
    JT _ZN18TCardConfigurationaSERK18TCardConfiguration, 01800d1c

/*
C++: RefVar::operator=(const long)
MPW-mangled: __as__6RefVarFCl
*/
    JT _ZN6RefVaraSEKl, 01802b90

/*
C++: RefVar::operator=(const RefVar&)
MPW-mangled: __as__6RefVarFRC6RefVar
*/
    JT _ZN6RefVaraSERK6RefVar, 01802c70

/*
C++: DataPtr::operator=(const DataPtr&)
MPW-mangled: __as__7DataPtrFRC7DataPtr
*/
    JT _ZN7DataPtraSERK7DataPtr, 01800798

/*
C++: DataPtr::operator=(long)
MPW-mangled: __as__7DataPtrFl
*/
    JT _ZN7DataPtraSEl, 01800794

/*
C++: TAddress::operator=(const TAddress&)
MPW-mangled: __as__8TAddressFRC8TAddress
*/
    JT _ZN8TAddressaSERK8TAddress, 0180207c

/*
C++: RefStruct::operator=(const long)
MPW-mangled: __as__9RefStructFCl
*/
    JT _ZN9RefStructaSEKl, 01802c80

/*
C++: RefStruct::operator=(const RefVar&)
MPW-mangled: __as__9RefStructFRC6RefVar
*/
    JT _ZN9RefStructaSERK6RefVar, 01802b94

/*
C++: CMemObject::CMemObject()
MPW-mangled: __ct__10CMemObjectFv
*/
    JT _ZN10CMemObjectC1Ev, 01801880

/*
C++: CMinBuffer::CMinBuffer()
MPW-mangled: __ct__10CMinBufferFv
*/
    JT _ZN10CMinBufferC1Ev, 01801884

/*
C++: TCircleBuf::TCircleBuf()
MPW-mangled: __ct__10TCircleBufFv
*/
    JT _ZN10TCircleBufC1Ev, 01802b48

/*
C++: TIdleTimer::TIdleTimer()
MPW-mangled: __ct__10TIdleTimerFv
*/
    JT _ZN10TIdleTimerC1Ev, 01801888

/*
C++: TObjectPtr::TObjectPtr(const TObjectPtr&)
MPW-mangled: __ct__10TObjectPtrFRC10TObjectPtr
*/
    JT _ZN10TObjectPtrC1ERK10TObjectPtr, 018007a0

/*
C++: TObjectPtr::TObjectPtr(const RefVar&)
MPW-mangled: __ct__10TObjectPtrFRC6RefVar
*/
    JT _ZN10TObjectPtrC1ERK6RefVar, 018007a4

/*
C++: TObjectPtr::TObjectPtr(const RefStruct&)
MPW-mangled: __ct__10TObjectPtrFRC9RefStruct
*/
    JT _ZN10TObjectPtrC1ERK9RefStruct, 018007a8

/*
C++: TObjectPtr::TObjectPtr(long)
MPW-mangled: __ct__10TObjectPtrFl
*/
    JT _ZN10TObjectPtrC1El, 0180079c

/*
C++: TObjectPtr::TObjectPtr()
MPW-mangled: __ct__10TObjectPtrFv
*/
    JT _ZN10TObjectPtrC1Ev, 018007ac

/*
C++: TPCommTool::TPCommTool(unsigned long, TCommToolProtocol*)
MPW-mangled: __ct__10TPCommToolFUlP17TCommToolProtocol
*/
    JT _ZN10TPCommToolC1EmP17TCommToolProtocol, 01800444

/*
C++: TPipeEvent::TPipeEvent()
MPW-mangled: __ct__10TPipeEventFv
*/
    JT _ZN10TPipeEventC1Ev, 01800d24

/*
C++: TStartInfo::TStartInfo()
MPW-mangled: __ct__10TStartInfoFv
*/
    JT _ZN10TStartInfoC1Ev, 018020b0

/*
C++: CBufferList::CBufferList()
MPW-mangled: __ct__11CBufferListFv
*/
    JT __ct__11CBufferListFv, 0180188c
    StaticNew 11CBufferList

/*
C++: CBufferPipe::CBufferPipe()
MPW-mangled: __ct__11CBufferPipeFv
*/
    JT _ZN11CBufferPipeC1Ev, 01801890

/*
C++: CRingBuffer::CRingBuffer()
MPW-mangled: __ct__11CRingBufferFv
*/
    JT _ZN11CRingBufferC1Ev, 01801894

/*
C++: CSortedList::CSortedList(CItemComparer*)
MPW-mangled: __ct__11CSortedListFP13CItemComparer
*/
    JT _ZN11CSortedListC1EP13CItemComparer, 01801898

/*
C++: TCardDevice::TCardDevice(unsigned long, unsigned long, unsigned long, unsigned char, unsigned char, unsigned char, unsigned char, unsigned long)
MPW-mangled: __ct__11TCardDeviceFUlN21UcN34T1
*/
    JT __ct__11TCardDeviceFUlN21UcN34T1, 01800d28

/*
C++: TCardDevice::TCardDevice()
MPW-mangled: __ct__11TCardDeviceFv
*/
    JT _ZN11TCardDeviceC1Ev, 01800d2c

/*
C++: TCardPCMCIA::TCardPCMCIA()
MPW-mangled: __ct__11TCardPCMCIAFv
*/
    JT _ZN11TCardPCMCIAC1Ev, 01800d30

/*
C++: TCardSocket::TCardSocket(unsigned long)
MPW-mangled: __ct__11TCardSocketFUl
*/
    JT _ZN11TCardSocketC1Em, 01800d34

/*
C++: TDelayTimer::TDelayTimer()
MPW-mangled: __ct__11TDelayTimerFv
*/
    JT _ZN11TDelayTimerC1Ev, 01802598

/*
C++: TGCSafeList::TGCSafeList()
MPW-mangled: __ct__11TGCSafeListFv
*/
    JT _ZN11TGCSafeListC1Ev, 0180189c

/*
C++: TPMIterator::TPMIterator()
MPW-mangled: __ct__11TPMIteratorFv
*/
    JT _ZN11TPMIteratorC1Ev, 01800d38

/*
C++: TUTaskWorld::TUTaskWorld()
MPW-mangled: __ct__11TUTaskWorldFv
*/
    JT _ZN11TUTaskWorldC1Ev, 01800d3c

/*
C++: TCMOTAPIHold::TCMOTAPIHold()
MPW-mangled: __ct__12TCMOTAPIHoldFv
*/
    JT _ZN12TCMOTAPIHoldC1Ev, 018024b8

/*
C++: TCardMessage::TCardMessage()
MPW-mangled: __ct__12TCardMessageFv
*/
    JT _ZN12TCardMessageC1Ev, 01800d40

/*
C++: TCardPackage::TCardPackage()
MPW-mangled: __ct__12TCardPackageFv
*/
    JT _ZN12TCardPackageC1Ev, 01800d44

/*
C++: TDoubleQItem::TDoubleQItem()
MPW-mangled: __ct__12TDoubleQItemFv
*/
    JT _ZN12TDoubleQItemC1Ev, 01800d48

/*
C++: TLookupNames::TLookupNames()
MPW-mangled: __ct__12TLookupNamesFv
*/
    JT _ZN12TLookupNamesC1Ev, 01802064

/*
C++: TOptionArray::TOptionArray()
MPW-mangled: __ct__12TOptionArrayFv
*/
    JT _ZN12TOptionArrayC1Ev, 018001b0

/*
C++: TP3OPassThru::TP3OPassThru(unsigned char)
MPW-mangled: __ct__12TP3OPassThruFUc
*/
    JT _ZN12TP3OPassThruC1Eh, 01800448

/*
C++: TPartHandler::TPartHandler()
MPW-mangled: __ct__12TPartHandlerFv
*/
    JT _ZN12TPartHandlerC1Ev, 01800d4c

/*
C++: TSystemEvent::TSystemEvent(unsigned long)
MPW-mangled: __ct__12TSystemEventFUl
*/
    JT _ZN12TSystemEventC1Em, 01800d50

/*
C++: TUNameServer::TUNameServer()
MPW-mangled: __ct__12TUNameServerFv
*/
    JT _ZN12TUNameServerC1Ev, 01800d54

/*
C++: CDynamicArray::CDynamicArray(long, long)
MPW-mangled: __ct__13CDynamicArrayFlT1
*/
    JT __ct__13CDynamicArrayFlT1, 018018a0

/*
C++: CDynamicArray::CDynamicArray()
MPW-mangled: __ct__13CDynamicArrayFv
*/
    JT _ZN13CDynamicArrayC1Ev, 018018a4

/*
C++: CItemComparer::CItemComparer(const void*, const void*)
MPW-mangled: __ct__13CItemComparerFPCvT1
*/
    JT __ct__13CItemComparerFPCvT1, 018018a8

/*
C++: CItemComparer::CItemComparer()
MPW-mangled: __ct__13CItemComparerFv
*/
    JT _ZN13CItemComparerC1Ev, 018018ac

/*
C++: CListIterator::CListIterator(CDynamicArray*)
MPW-mangled: __ct__13CListIteratorFP13CDynamicArray
*/
    JT _ZN13CListIteratorC1EP13CDynamicArray, 018018b0

/*
C++: CListIterator::CListIterator(CDynamicArray*, unsigned char)
MPW-mangled: __ct__13CListIteratorFP13CDynamicArrayUc
*/
    JT _ZN13CListIteratorC1EP13CDynamicArrayh, 018018b8

/*
C++: CListIterator::CListIterator(CDynamicArray*, long, long, unsigned char)
MPW-mangled: __ct__13CListIteratorFP13CDynamicArraylT2Uc
*/
    JT __ct__13CListIteratorFP13CDynamicArraylT2Uc, 018018b4

/*
C++: CListIterator::CListIterator()
MPW-mangled: __ct__13CListIteratorFv
*/
    JT _ZN13CListIteratorC1Ev, 018018bc

/*
C++: TCMOIdleTimer::TCMOIdleTimer()
MPW-mangled: __ct__13TCMOIdleTimerFv
*/
    JT _ZN13TCMOIdleTimerC1Ev, 01800454

/*
C++: TCardFunction::TCardFunction()
MPW-mangled: __ct__13TCardFunctionFv
*/
    JT _ZN13TCardFunctionC1Ev, 01802da8

/*
C++: TCardLongLink::TCardLongLink()
MPW-mangled: __ct__13TCardLongLinkFv
*/
    JT _ZN13TCardLongLinkC1Ev, 01800d58

/*
C++: TEndpointPipe::TEndpointPipe()
MPW-mangled: __ct__13TEndpointPipeFv
*/
    JT _ZN13TEndpointPipeC1Ev, 018001b4

/*
C++: TMemoryObject::TMemoryObject()
MPW-mangled: __ct__13TMemoryObjectFv
*/
    JT _ZN13TMemoryObjectC1Ev, 01802080

/*
C++: TObjectReader::TObjectReader(CPipe&)
MPW-mangled: __ct__13TObjectReaderFR5CPipe
*/
    JT _ZN13TObjectReaderC1ER5CPipe, 018007b4

/*
C++: TObjectWriter::TObjectWriter(const RefVar&, CPipe&, int)
MPW-mangled: __ct__13TObjectWriterFRC6RefVarR5CPipei
*/
    JT _ZN13TObjectWriterC1ERK6RefVarR5CPipei, 018007b8

/*
C++: CArrayIterator::CArrayIterator(CDynamicArray*)
MPW-mangled: __ct__14CArrayIteratorFP13CDynamicArray
*/
    JT _ZN14CArrayIteratorC1EP13CDynamicArray, 018018c0

/*
C++: CArrayIterator::CArrayIterator(CDynamicArray*, unsigned char)
MPW-mangled: __ct__14CArrayIteratorFP13CDynamicArrayUc
*/
    JT _ZN14CArrayIteratorC1EP13CDynamicArrayh, 018018c8

/*
C++: CArrayIterator::CArrayIterator(CDynamicArray*, long, long, unsigned char)
MPW-mangled: __ct__14CArrayIteratorFP13CDynamicArraylT2Uc
*/
    JT __ct__14CArrayIteratorFP13CDynamicArraylT2Uc, 018018c4

/*
C++: CArrayIterator::CArrayIterator()
MPW-mangled: __ct__14CArrayIteratorFv
*/
    JT _ZN14CArrayIteratorC1Ev, 018018cc

/*
C++: CBufferSegment::CBufferSegment()
MPW-mangled: __ct__14CBufferSegmentFv
*/
    JT __ct__14CBufferSegmentFv, 018018d0
    StaticNew 14CBufferSegment

/*
C++: TAEventHandler::TAEventHandler()
MPW-mangled: __ct__14TAEventHandlerFv
*/
    JT _ZN14TAEventHandlerC1Ev, 018018d4

/*
C++: TAppWorldState::TAppWorldState()
MPW-mangled: __ct__14TAppWorldStateFv
*/
    JT _ZN14TAppWorldStateC1Ev, 018018d8

/*
C++: TCMOFaxLocalId::TCMOFaxLocalId()
MPW-mangled: __ct__14TCMOFaxLocalIdFv
*/
    JT _ZN14TCMOFaxLocalIdC1Ev, 01800458

/*
C++: TCMOModemPrefs::TCMOModemPrefs()
MPW-mangled: __ct__14TCMOModemPrefsFv
*/
    JT _ZN14TCMOModemPrefsC1Ev, 0180045c

/*
C++: TCMOTAPIAnswer::TCMOTAPIAnswer()
MPW-mangled: __ct__14TCMOTAPIAnswerFv
*/
    JT _ZN14TCMOTAPIAnswerC1Ev, 01802908

/*
C++: TCMOTAPIUnhold::TCMOTAPIUnhold()
MPW-mangled: __ct__14TCMOTAPIUnholdFv
*/
    JT _ZN14TCMOTAPIUnholdC1Ev, 0180290c

/*
C++: TCardATALoader::TCardATALoader()
MPW-mangled: __ct__14TCardATALoaderFv
*/
    JT _ZN14TCardATALoaderC1Ev, 01802b04

/*
C++: TCommToolReply::TCommToolReply()
MPW-mangled: __ct__14TCommToolReplyFv
*/
    JT _ZN14TCommToolReplyC1Ev, 01801e80

/*
C++: TInstalledPart::TInstalledPart(unsigned long, long, long, unsigned char, unsigned char, unsigned char, unsigned char, unsigned long)
MPW-mangled: __ct__14TInstalledPartFUllT2UcN34T1
*/
    JT __ct__14TInstalledPartFUllT2UcN34T1, 01800d5c

/*
C++: TPackageLoader::TPackageLoader(TEndpointPipe*, SourceType)
MPW-mangled: __ct__14TPackageLoaderFP13TEndpointPipe10SourceType
*/
    JT _ZN14TPackageLoaderC1EP13TEndpointPipe10SourceType, 01800d64

/*
C++: TPackageLoader::TPackageLoader(CPipe*, SourceType)
MPW-mangled: __ct__14TPackageLoaderFP5CPipe10SourceType
*/
    JT _ZN14TPackageLoaderC1EP5CPipe10SourceType, 01800d68

/*
C++: TPackageLoader::TPackageLoader(char*, SourceType)
MPW-mangled: __ct__14TPackageLoaderFPc10SourceType
*/
    JT _ZN14TPackageLoaderC1EPc10SourceType, 01800d6c

/*
C++: TUAsyncMessage::TUAsyncMessage(const TUAsyncMessage&)
MPW-mangled: __ct__14TUAsyncMessageFRC14TUAsyncMessage
*/
    JT _ZN14TUAsyncMessageC1ERK14TUAsyncMessage, 01800d70

/*
C++: TUAsyncMessage::TUAsyncMessage(unsigned long, unsigned long)
MPW-mangled: __ct__14TUAsyncMessageFUlT1
*/
    JT __ct__14TUAsyncMessageFUlT1, 01800d74

/*
C++: TUAsyncMessage::TUAsyncMessage()
MPW-mangled: __ct__14TUAsyncMessageFv
*/
    JT _ZN14TUAsyncMessageC1Ev, 01800d78

/*
C++: TUSoundChannel::TUSoundChannel()
MPW-mangled: __ct__14TUSoundChannelFv
*/
    JT _ZN14TUSoundChannelC1Ev, 01802c5c

/*
C++: CBaseRingBuffer::CBaseRingBuffer()
MPW-mangled: __ct__15CBaseRingBufferFv
*/
    JT _ZN15CBaseRingBufferC1Ev, 018018dc

/*
C++: TCMOFaxPassThru::TCMOFaxPassThru()
MPW-mangled: __ct__15TCMOFaxPassThruFv
*/
    JT _ZN15TCMOFaxPassThruC1Ev, 01800460

/*
C++: TCMOFaxRemoteId::TCMOFaxRemoteId()
MPW-mangled: __ct__15TCMOFaxRemoteIdFv
*/
    JT _ZN15TCMOFaxRemoteIdC1Ev, 01800464

/*
C++: TCMOListenTimer::TCMOListenTimer()
MPW-mangled: __ct__15TCMOListenTimerFv
*/
    JT _ZN15TCMOListenTimerC1Ev, 01800468

/*
C++: TCMOMNPAllocate::TCMOMNPAllocate()
MPW-mangled: __ct__15TCMOMNPAllocateFv
*/
    JT _ZN15TCMOMNPAllocateC1Ev, 0180046c

/*
C++: TCMOMNPDataRate::TCMOMNPDataRate()
MPW-mangled: __ct__15TCMOMNPDataRateFv
*/
    JT _ZN15TCMOMNPDataRateC1Ev, 01800470

/*
C++: TCMOModemECType::TCMOModemECType()
MPW-mangled: __ct__15TCMOModemECTypeFv
*/
    JT _ZN15TCMOModemECTypeC1Ev, 01800474

/*
C++: TCMOSerialBreak::TCMOSerialBreak()
MPW-mangled: __ct__15TCMOSerialBreakFv
*/
    JT _ZN15TCMOSerialBreakC1Ev, 01800478

/*
C++: TCMOSlowIRSniff::TCMOSlowIRSniff()
MPW-mangled: __ct__15TCMOSlowIRSniffFv
*/
    JT _ZN15TCMOSlowIRSniffC1Ev, 0180047c

/*
C++: TCMOSlowIRStats::TCMOSlowIRStats()
MPW-mangled: __ct__15TCMOSlowIRStatsFv
*/
    JT _ZN15TCMOSlowIRStatsC1Ev, 01800480

/*
C++: TCMOTAPIForward::TCMOTAPIForward()
MPW-mangled: __ct__15TCMOTAPIForwardFv
*/
    JT _ZN15TCMOTAPIForwardC1Ev, 01802910

/*
C++: TCMOTAPIService::TCMOTAPIService()
MPW-mangled: __ct__15TCMOTAPIServiceFv
*/
    JT _ZN15TCMOTAPIServiceC1Ev, 01802914

/*
C++: TCMOTAPISpeaker::TCMOTAPISpeaker()
MPW-mangled: __ct__15TCMOTAPISpeakerFv
*/
    JT _ZN15TCMOTAPISpeakerC1Ev, 01802918

/*
C++: TCommToolAEvent::TCommToolAEvent()
MPW-mangled: __ct__15TCommToolAEventFv
*/
    JT _ZN15TCommToolAEventC1Ev, 01801e98

/*
C++: TEndpointClient::TEndpointClient()
MPW-mangled: __ct__15TEndpointClientFv
*/
    JT _ZN15TEndpointClientC1Ev, 018001b8

/*
C++: TEzEndpointPipe::TEzEndpointPipe(unsigned char)
MPW-mangled: __ct__15TEzEndpointPipeFUc
*/
    JT _ZN15TEzEndpointPipeC1Eh, 018001bc

/*
C++: TEzEndpointPipe::TEzEndpointPipe()
MPW-mangled: __ct__15TEzEndpointPipeFv
*/
    JT _ZN15TEzEndpointPipeC1Ev, 018001c0

/*
C++: TObjectIterator::TObjectIterator(const RefVar&, int)
MPW-mangled: __ct__15TObjectIteratorFRC6RefVari
*/
    JT _ZN15TObjectIteratorC1ERK6RefVari, 018007bc

/*
C++: TOptionExtended::TOptionExtended(unsigned long)
MPW-mangled: __ct__15TOptionExtendedFUl
*/
    JT _ZN15TOptionExtendedC1Em, 01802b14

/*
C++: TOptionIterator::TOptionIterator(TOptionArray*)
MPW-mangled: __ct__15TOptionIteratorFP12TOptionArray
*/
    JT _ZN15TOptionIteratorC1EP12TOptionArray, 018001c4

/*
C++: TOptionIterator::TOptionIterator(TOptionArray*, long, long)
MPW-mangled: __ct__15TOptionIteratorFP12TOptionArraylT2
*/
    JT __ct__15TOptionIteratorFP12TOptionArraylT2, 018001c8

/*
C++: TOptionIterator::TOptionIterator()
MPW-mangled: __ct__15TOptionIteratorFv
*/
    JT _ZN15TOptionIteratorC1Ev, 018001cc

/*
C++: TPCMCIA20Parser::TPCMCIA20Parser()
MPW-mangled: __ct__15TPCMCIA20ParserFv
*/
    JT _ZN15TPCMCIA20ParserC1Ev, 01802580

/*
C++: TStoreHashTable::TStoreHashTable(TStore*, unsigned long)
MPW-mangled: __ct__15TStoreHashTableFP6TStoreUl
*/
    JT _ZN15TStoreHashTableC1EP6TStorem, 018007c0

/*
C++: TUDomainManager::TUDomainManager()
MPW-mangled: __ct__15TUDomainManagerFv
*/
    JT _ZN15TUDomainManagerC1Ev, 01800d7c

/*
C++: TCMOBreakFraming::TCMOBreakFraming()
MPW-mangled: __ct__16TCMOBreakFramingFv
*/
    JT _ZN16TCMOBreakFramingC1Ev, 01800490

/*
C++: TCMOEndpointName::TCMOEndpointName()
MPW-mangled: __ct__16TCMOEndpointNameFv
*/
    JT _ZN16TCMOEndpointNameC1Ev, 018001d0

/*
C++: TCMOFaxDirection::TCMOFaxDirection()
MPW-mangled: __ct__16TCMOFaxDirectionFv
*/
    JT _ZN16TCMOFaxDirectionC1Ev, 01800494

/*
C++: TCMOFaxPageSetUp::TCMOFaxPageSetUp()
MPW-mangled: __ct__16TCMOFaxPageSetUpFv
*/
    JT _ZN16TCMOFaxPageSetUpC1Ev, 01800498

/*
C++: TCMOFaxStartPage::TCMOFaxStartPage()
MPW-mangled: __ct__16TCMOFaxStartPageFv
*/
    JT _ZN16TCMOFaxStartPageC1Ev, 0180291c

/*
C++: TCMOFramingParms::TCMOFramingParms()
MPW-mangled: __ct__16TCMOFramingParmsFv
*/
    JT _ZN16TCMOFramingParmsC1Ev, 0180049c

/*
C++: TCMOModemDialing::TCMOModemDialing()
MPW-mangled: __ct__16TCMOModemDialingFv
*/
    JT _ZN16TCMOModemDialingC1Ev, 018004a0

/*
C++: TCMOModemProfile::TCMOModemProfile(unsigned long)
MPW-mangled: __ct__16TCMOModemProfileFUl
*/
    JT _ZN16TCMOModemProfileC1Em, 018004a4

/*
C++: TCMOPassiveClaim::TCMOPassiveClaim()
MPW-mangled: __ct__16TCMOPassiveClaimFv
*/
    JT _ZN16TCMOPassiveClaimC1Ev, 01802920

/*
C++: TCMOPassiveState::TCMOPassiveState()
MPW-mangled: __ct__16TCMOPassiveStateFv
*/
    JT _ZN16TCMOPassiveStateC1Ev, 01802924

/*
C++: TCMOTAPIOutGoing::TCMOTAPIOutGoing()
MPW-mangled: __ct__16TCMOTAPIOutGoingFv
*/
    JT _ZN16TCMOTAPIOutGoingC1Ev, 01802928

/*
C++: TCMOTAPITransfer::TCMOTAPITransfer()
MPW-mangled: __ct__16TCMOTAPITransferFv
*/
    JT _ZN16TCMOTAPITransferC1Ev, 0180292c

/*
C++: TCPNewtonIdTuple::TCPNewtonIdTuple()
MPW-mangled: __ct__16TCPNewtonIdTupleFv
*/
    JT _ZN16TCPNewtonIdTupleC1Ev, 018001d4

/*
C++: TCardCISIterator::TCardCISIterator()
MPW-mangled: __ct__16TCardCISIteratorFv
*/
    JT _ZN16TCardCISIteratorC1Ev, 018024e8

/*
C++: TDisconnectEvent::TDisconnectEvent(long, unsigned long)
MPW-mangled: __ct__16TDisconnectEventFlUl
*/
    JT _ZN16TDisconnectEventC1Elm, 018001d8

/*
C++: TPackageIterator::TPackageIterator(CPipe*)
MPW-mangled: __ct__16TPackageIteratorFP5CPipe
*/
    JT _ZN16TPackageIteratorC1EP5CPipe, 01800d80

/*
C++: TPackageIterator::TPackageIterator(void*)
MPW-mangled: __ct__16TPackageIteratorFPv
*/
    JT _ZN16TPackageIteratorC1EPv, 01800d84

/*
C++: CShadowRingBuffer::CShadowRingBuffer()
MPW-mangled: __ct__17CShadowRingBufferFv
*/
    JT _ZN17CShadowRingBufferC1Ev, 018018e0

/*
C++: TATAPartitionInfo::TATAPartitionInfo()
MPW-mangled: __ct__17TATAPartitionInfoFv
*/
    JT _ZN17TATAPartitionInfoC1Ev, 01802514

/*
C++: TCMOCTConnectInfo::TCMOCTConnectInfo()
MPW-mangled: __ct__17TCMOCTConnectInfoFv
*/
    JT _ZN17TCMOCTConnectInfoC1Ev, 018004a8

/*
C++: TCMOFaxEndMessage::TCMOFaxEndMessage()
MPW-mangled: __ct__17TCMOFaxEndMessageFv
*/
    JT _ZN17TCMOFaxEndMessageC1Ev, 01802930

/*
C++: TCMOMNPStatistics::TCMOMNPStatistics()
MPW-mangled: __ct__17TCMOMNPStatisticsFv
*/
    JT _ZN17TCMOMNPStatisticsC1Ev, 018004ac

/*
C++: TCMOSerialBitRate::TCMOSerialBitRate()
MPW-mangled: __ct__17TCMOSerialBitRateFv
*/
    JT _ZN17TCMOSerialBitRateC1Ev, 018004b0

/*
C++: TCMOSerialBuffers::TCMOSerialBuffers()
MPW-mangled: __ct__17TCMOSerialBuffersFv
*/
    JT _ZN17TCMOSerialBuffersC1Ev, 018004b4

/*
C++: TCMOSerialDiscard::TCMOSerialDiscard()
MPW-mangled: __ct__17TCMOSerialDiscardFv
*/
    JT _ZN17TCMOSerialDiscardC1Ev, 018004b8

/*
C++: TCMOSerialIOParms::TCMOSerialIOParms()
MPW-mangled: __ct__17TCMOSerialIOParmsFv
*/
    JT _ZN17TCMOSerialIOParmsC1Ev, 018004bc

/*
C++: TCMOSerialIOStats::TCMOSerialIOStats()
MPW-mangled: __ct__17TCMOSerialIOStatsFv
*/
    JT _ZN17TCMOSerialIOStatsC1Ev, 018004c0

/*
C++: TCMOSlowIRBitBang::TCMOSlowIRBitBang()
MPW-mangled: __ct__17TCMOSlowIRBitBangFv
*/
    JT _ZN17TCMOSlowIRBitBangC1Ev, 018004c4

/*
C++: TCMOSlowIRConnect::TCMOSlowIRConnect()
MPW-mangled: __ct__17TCMOSlowIRConnectFv
*/
    JT _ZN17TCMOSlowIRConnectC1Ev, 01802934

/*
C++: TCMOTAPISendDigit::TCMOTAPISendDigit()
MPW-mangled: __ct__17TCMOTAPISendDigitFv
*/
    JT _ZN17TCMOTAPISendDigitC1Ev, 01802938

/*
C++: TCMOTransportInfo::TCMOTransportInfo()
MPW-mangled: __ct__17TCMOTransportInfoFv
*/
    JT _ZN17TCMOTransportInfoC1Ev, 018001dc

/*
C++: TCommToolGetReply::TCommToolGetReply()
MPW-mangled: __ct__17TCommToolGetReplyFv
*/
    JT _ZN17TCommToolGetReplyC1Ev, 01801e88

/*
C++: TCommToolPutReply::TCommToolPutReply()
MPW-mangled: __ct__17TCommToolPutReplyFv
*/
    JT _ZN17TCommToolPutReplyC1Ev, 01801e84

/*
C++: TDoubleQContainer::TDoubleQContainer(char*)
MPW-mangled: __ct__17TDoubleQContainerFPc
*/
    JT _ZN17TDoubleQContainerC1EPc, 01800d88

/*
C++: TDoubleQContainer::TDoubleQContainer(char*, void (*)(void*, void*), void*)
MPW-mangled: __ct__17TDoubleQContainerFPcPFPvT1_vPv
*/
    JT __ct__17TDoubleQContainerFPcPFPvT1_vPv, 01800d8c

/*
C++: TGCSafeSortedList::TGCSafeSortedList(CItemComparer*)
MPW-mangled: __ct__17TGCSafeSortedListFP13CItemComparer
*/
    JT _ZN17TGCSafeSortedListC1EP13CItemComparer, 018018e4

/*
C++: TLicenseeVAddress::TLicenseeVAddress(const TLicenseeVAddress&)
MPW-mangled: __ct__17TLicenseeVAddressFRC17TLicenseeVAddress
*/
    JT _ZN17TLicenseeVAddressC1ERK17TLicenseeVAddress, 01802b08

/*
C++: TLicenseeVAddress::TLicenseeVAddress(unsigned long)
MPW-mangled: __ct__17TLicenseeVAddressFUl
*/
    JT _ZN17TLicenseeVAddressC1Em, 01802554

/*
C++: TUserAbortHandler::TUserAbortHandler()
MPW-mangled: __ct__17TUserAbortHandlerFv
*/
    JT _ZN17TUserAbortHandlerC1Ev, 018001e0

/*
C++: TAEHandlerComparer::TAEHandlerComparer()
MPW-mangled: __ct__18TAEHandlerComparerFv
*/
    JT _ZN18TAEHandlerComparerC1Ev, 018018e8

/*
C++: TAEHandlerIterator::TAEHandlerIterator(TAEventHandler*)
MPW-mangled: __ct__18TAEHandlerIteratorFP14TAEventHandler
*/
    JT _ZN18TAEHandlerIteratorC1EP14TAEventHandler, 018018ec

/*
C++: TATABootParamBlock::TATABootParamBlock()
MPW-mangled: __ct__18TATABootParamBlockFv
*/
    JT _ZN18TATABootParamBlockC1Ev, 0180251c

/*
C++: TCMOFaxSessionInfo::TCMOFaxSessionInfo()
MPW-mangled: __ct__18TCMOFaxSessionInfoFv
*/
    JT _ZN18TCMOFaxSessionInfoC1Ev, 018004cc

/*
C++: TCMOLocalTalkStats::TCMOLocalTalkStats()
MPW-mangled: __ct__18TCMOLocalTalkStatsFv
*/
    JT _ZN18TCMOLocalTalkStatsC1Ev, 018004d0

/*
C++: TCMOMNPCompression::TCMOMNPCompression()
MPW-mangled: __ct__18TCMOMNPCompressionFv
*/
    JT _ZN18TCMOMNPCompressionC1Ev, 018004d4

/*
C++: TCMOSerialChipSpec::TCMOSerialChipSpec()
MPW-mangled: __ct__18TCMOSerialChipSpecFv
*/
    JT _ZN18TCMOSerialChipSpecC1Ev, 0180293c

/*
C++: TCMOSerialHardware::TCMOSerialHardware()
MPW-mangled: __ct__18TCMOSerialHardwareFv
*/
    JT _ZN18TCMOSerialHardwareC1Ev, 018004d8

/*
C++: TCMOTAPIDisconnect::TCMOTAPIDisconnect()
MPW-mangled: __ct__18TCMOTAPIDisconnectFv
*/
    JT _ZN18TCMOTAPIDisconnectC1Ev, 01802940

/*
C++: TCardConfiguration::TCardConfiguration()
MPW-mangled: __ct__18TCardConfigurationFv
*/
    JT _ZN18TCardConfigurationC1Ev, 01800d90

/*
C++: TCommToolBindReply::TCommToolBindReply()
MPW-mangled: __ct__18TCommToolBindReplyFv
*/
    JT _ZN18TCommToolBindReplyC1Ev, 01801e90

/*
C++: TCMOLocalTalkNodeID::TCMOLocalTalkNodeID()
MPW-mangled: __ct__19TCMOLocalTalkNodeIDFv
*/
    JT _ZN19TCMOLocalTalkNodeIDC1Ev, 018004dc

/*
C++: TCMOMNPDebugConnect::TCMOMNPDebugConnect()
MPW-mangled: __ct__19TCMOMNPDebugConnectFv
*/
    JT _ZN19TCMOMNPDebugConnectC1Ev, 01802944

/*
C++: TCMOSerialHWChipLoc::TCMOSerialHWChipLoc()
MPW-mangled: __ct__19TCMOSerialHWChipLocFv
*/
    JT _ZN19TCMOSerialHWChipLocC1Ev, 01802948

/*
C++: TCommToolGetRequest::TCommToolGetRequest()
MPW-mangled: __ct__19TCommToolGetRequestFv
*/
    JT _ZN19TCommToolGetRequestC1Ev, 01801ea4

/*
C++: TCommToolOptionInfo::TCommToolOptionInfo()
MPW-mangled: __ct__19TCommToolOptionInfoFv
*/
    JT _ZN19TCommToolOptionInfoC1Ev, 018026c0

/*
C++: TCommToolPutRequest::TCommToolPutRequest()
MPW-mangled: __ct__19TCommToolPutRequestFv
*/
    JT _ZN19TCommToolPutRequestC1Ev, 01801ea0

/*
C++: TOSErrorAlertDialog::TOSErrorAlertDialog()
MPW-mangled: __ct__19TOSErrorAlertDialogFv
*/
    JT _ZN19TOSErrorAlertDialogC1Ev, 01802794

/*
C++: TSystemEventHandler::TSystemEventHandler()
MPW-mangled: __ct__19TSystemEventHandlerFv
*/
    JT _ZN19TSystemEventHandlerC1Ev, 018018f0

/*
C++: TUSoundCallbackProc::TUSoundCallbackProc()
MPW-mangled: __ct__19TUSoundCallbackProcFv
*/
    JT _ZN19TUSoundCallbackProcC1Ev, 01802c60

/*
C++: CShadowBufferSegment::CShadowBufferSegment()
MPW-mangled: __ct__20CShadowBufferSegmentFv
*/
    JT _ZN20CShadowBufferSegmentC1Ev, 018018f4

/*
C++: TCMOFlowControlParms::TCMOFlowControlParms()
MPW-mangled: __ct__20TCMOFlowControlParmsFv
*/
    JT _ZN20TCMOFlowControlParmsC1Ev, 018004e0

/*
C++: TCMOFramedAsyncStats::TCMOFramedAsyncStats()
MPW-mangled: __ct__20TCMOFramedAsyncStatsFv
*/
    JT _ZN20TCMOFramedAsyncStatsC1Ev, 018004e4

/*
C++: TCMOModemConnectType::TCMOModemConnectType()
MPW-mangled: __ct__20TCMOModemConnectTypeFv
*/
    JT _ZN20TCMOModemConnectTypeC1Ev, 018004e8

/*
C++: TCMOSerialDTRControl::TCMOSerialDTRControl()
MPW-mangled: __ct__20TCMOSerialDTRControlFv
*/
    JT _ZN20TCMOSerialDTRControlC1Ev, 018004ec

/*
C++: TCMOSerialHalfDuplex::TCMOSerialHalfDuplex()
MPW-mangled: __ct__20TCMOSerialHalfDuplexFv
*/
    JT _ZN20TCMOSerialHalfDuplexC1Ev, 0180294c

/*
C++: TCMOSerialMiscConfig::TCMOSerialMiscConfig()
MPW-mangled: __ct__20TCMOSerialMiscConfigFv
*/
    JT _ZN20TCMOSerialMiscConfigC1Ev, 018004f0

/*
C++: TCMOTAPIForwardClear::TCMOTAPIForwardClear()
MPW-mangled: __ct__20TCMOTAPIForwardClearFv
*/
    JT _ZN20TCMOTAPIForwardClearC1Ev, 01802950

/*
C++: TCommToolBindRequest::TCommToolBindRequest()
MPW-mangled: __ct__20TCommToolBindRequestFv
*/
    JT _ZN20TCommToolBindRequestC1Ev, 01801eb4

/*
C++: TCommToolKillRequest::TCommToolKillRequest()
MPW-mangled: __ct__20TCommToolKillRequestFv
*/
    JT _ZN20TCommToolKillRequestC1Ev, 01801ec0

/*
C++: TCommToolOpenRequest::TCommToolOpenRequest()
MPW-mangled: __ct__20TCommToolOpenRequestFv
*/
    JT _ZN20TCommToolOpenRequestC1Ev, 01801ea8

/*
C++: TCMOFaxConfigSendBand::TCMOFaxConfigSendBand()
MPW-mangled: __ct__21TCMOFaxConfigSendBandFv
*/
    JT _ZN21TCMOFaxConfigSendBandC1Ev, 01802954

/*
C++: TCMOHandsetManagement::TCMOHandsetManagement()
MPW-mangled: __ct__21TCMOHandsetManagementFv
*/
    JT _ZN21TCMOHandsetManagementC1Ev, 01802958

/*
C++: TCMOModemConnectSpeed::TCMOModemConnectSpeed()
MPW-mangled: __ct__21TCMOModemConnectSpeedFv
*/
    JT _ZN21TCMOModemConnectSpeedC1Ev, 0180295c

/*
C++: TCMOModemVoiceSupport::TCMOModemVoiceSupport()
MPW-mangled: __ct__21TCMOModemVoiceSupportFv
*/
    JT _ZN21TCMOModemVoiceSupportC1Ev, 018004f4

/*
C++: TCMOServiceIdentifier::TCMOServiceIdentifier()
MPW-mangled: __ct__21TCMOServiceIdentifierFv
*/
    JT _ZN21TCMOServiceIdentifierC1Ev, 01802b18

/*
C++: TCommToolConnectReply::TCommToolConnectReply()
MPW-mangled: __ct__21TCommToolConnectReplyFv
*/
    JT _ZN21TCommToolConnectReplyC1Ev, 01801e8c

/*
C++: TEndpointEventHandler::TEndpointEventHandler(TEndpoint*, unsigned char)
MPW-mangled: __ct__21TEndpointEventHandlerFP9TEndpointUc
*/
    JT _ZN21TEndpointEventHandlerC1EP9TEndpointh, 018001e4

/*
C++: TOptMgmtCompleteEvent::TOptMgmtCompleteEvent(long, unsigned long)
MPW-mangled: __ct__21TOptMgmtCompleteEventFlUl
*/
    JT _ZN21TOptMgmtCompleteEventC1Elm, 018001e8

/*
C++: TCMANamedAppleTalkAddr::TCMANamedAppleTalkAddr(unsigned long, long)
MPW-mangled: __ct__22TCMANamedAppleTalkAddrFUll
*/
    JT _ZN22TCMANamedAppleTalkAddrC1Eml, 018001ec

/*
C++: TCMOFaxMinScanLineTime::TCMOFaxMinScanLineTime()
MPW-mangled: __ct__22TCMOFaxMinScanLineTimeFv
*/
    JT _ZN22TCMOFaxMinScanLineTimeC1Ev, 01802960

/*
C++: TCMOSerialEventEnables::TCMOSerialEventEnables()
MPW-mangled: __ct__22TCMOSerialEventEnablesFv
*/
    JT _ZN22TCMOSerialEventEnablesC1Ev, 018004f8

/*
C++: TCMOSlowIRProtocolType::TCMOSlowIRProtocolType()
MPW-mangled: __ct__22TCMOSlowIRProtocolTypeFv
*/
    JT _ZN22TCMOSlowIRProtocolTypeC1Ev, 018004fc

/*
C++: TCommToolGetEventReply::TCommToolGetEventReply()
MPW-mangled: __ct__22TCommToolGetEventReplyFv
*/
    JT _ZN22TCommToolGetEventReplyC1Ev, 01801e94

/*
C++: TCommToolStatusRequest::TCommToolStatusRequest()
MPW-mangled: __ct__22TCommToolStatusRequestFv
*/
    JT _ZN22TCommToolStatusRequestC1Ev, 01801ec4

/*
C++: TCMOLocalTalkMiscConfig::TCMOLocalTalkMiscConfig()
MPW-mangled: __ct__23TCMOLocalTalkMiscConfigFv
*/
    JT _ZN23TCMOLocalTalkMiscConfigC1Ev, 01800500

/*
C++: TCMOMNPSpeedNegotiation::TCMOMNPSpeedNegotiation()
MPW-mangled: __ct__23TCMOMNPSpeedNegotiationFv
*/
    JT _ZN23TCMOMNPSpeedNegotiationC1Ev, 01802964

/*
C++: TCMOToolSpecificOptions::TCMOToolSpecificOptions()
MPW-mangled: __ct__23TCMOToolSpecificOptionsFv
*/
    JT _ZN23TCMOToolSpecificOptionsC1Ev, 01802968

/*
C++: TCommToolConnectRequest::TCommToolConnectRequest()
MPW-mangled: __ct__23TCommToolConnectRequestFv
*/
    JT _ZN23TCommToolConnectRequestC1Ev, 01801eac

/*
C++: TCommToolControlRequest::TCommToolControlRequest()
MPW-mangled: __ct__23TCommToolControlRequestFv
*/
    JT _ZN23TCommToolControlRequestC1Ev, 01801e9c

/*
C++: TStoreHashTableIterator::TStoreHashTableIterator(TStoreHashTable*)
MPW-mangled: __ct__23TStoreHashTableIteratorFP15TStoreHashTable
*/
    JT _ZN23TStoreHashTableIteratorC1EP15TStoreHashTable, 018007c4

/*
C++: TCMOModemFaxCapabilities::TCMOModemFaxCapabilities()
MPW-mangled: __ct__24TCMOModemFaxCapabilitiesFv
*/
    JT _ZN24TCMOModemFaxCapabilitiesC1Ev, 0180296c

/*
C++: TCMOSerialBytesAvailable::TCMOSerialBytesAvailable()
MPW-mangled: __ct__24TCMOSerialBytesAvailableFv
*/
    JT _ZN24TCMOSerialBytesAvailableC1Ev, 01800504

/*
C++: TCMOSerialCircuitControl::TCMOSerialCircuitControl()
MPW-mangled: __ct__24TCMOSerialCircuitControlFv
*/
    JT _ZN24TCMOSerialCircuitControlC1Ev, 01802970

/*
C++: TCMOInputFlowControlParms::TCMOInputFlowControlParms()
MPW-mangled: __ct__25TCMOInputFlowControlParmsFv
*/
    JT _ZN25TCMOInputFlowControlParmsC1Ev, 01800508

/*
C++: TGetProtAddrCompleteEvent::TGetProtAddrCompleteEvent(long, unsigned long)
MPW-mangled: __ct__25TGetProtAddrCompleteEventFlUl
*/
    JT _ZN25TGetProtAddrCompleteEventC1Elm, 018001f0

/*
C++: TCMOFaxEnableProgressEvent::TCMOFaxEnableProgressEvent()
MPW-mangled: __ct__26TCMOFaxEnableProgressEventFv
*/
    JT _ZN26TCMOFaxEnableProgressEventC1Ev, 01802974

/*
C++: TCMOOutputFlowControlParms::TCMOOutputFlowControlParms()
MPW-mangled: __ct__26TCMOOutputFlowControlParmsFv
*/
    JT _ZN26TCMOOutputFlowControlParmsC1Ev, 0180050c

/*
C++: TCommToolDisconnectRequest::TCommToolDisconnectRequest()
MPW-mangled: __ct__26TCommToolDisconnectRequestFv
*/
    JT _ZN26TCommToolDisconnectRequestC1Ev, 01801eb0

/*
C++: TCommToolOptionMgmtRequest::TCommToolOptionMgmtRequest()
MPW-mangled: __ct__26TCommToolOptionMgmtRequestFv
*/
    JT _ZN26TCommToolOptionMgmtRequestC1Ev, 01801eb8

/*
C++: TCommToolGetProtAddrRequest::TCommToolGetProtAddrRequest()
MPW-mangled: __ct__27TCommToolGetProtAddrRequestFv
*/
    JT _ZN27TCommToolGetProtAddrRequestC1Ev, 01801ebc

/*
C++: CList::CList()
MPW-mangled: __ct__5CListFv
*/
    JT _ZN5CListC1Ev, 018018f8

/*
C++: CPipe::CPipe()
MPW-mangled: __ct__5CPipeFv
*/
    JT _ZN5CPipeC1Ev, 018018fc

/*
C++: TDate::TDate(const unsigned short*, unsigned long*, unsigned long)
MPW-mangled: __ct__5TDateFPCUsPUlUl
*/
    JT _ZN5TDateC1EPKtPmm, 018025f4

/*
C++: TDate::TDate(unsigned long)
MPW-mangled: __ct__5TDateFUl
*/
    JT _ZN5TDateC1Em, 01802604

/*
C++: TDate::TDate()
MPW-mangled: __ct__5TDateFv
*/
    JT _ZN5TDateC1Ev, 01802608

/*
C++: TTime::TTime(unsigned long, TimeUnits)
MPW-mangled: __ct__5TTimeFUl9TimeUnits
*/
    JT _ZN5TTimeC1Em9TimeUnits, 01800d94

/*
C++: RefVar::RefVar(const long)
MPW-mangled: __ct__6RefVarFCl
*/
    JT _ZN6RefVarC1EKl, 01802b80

/*
C++: RefVar::RefVar(const RefVar&)
MPW-mangled: __ct__6RefVarFRC6RefVar
*/
    JT _ZN6RefVarC1ERK6RefVar, 01802c6c

/*
C++: RefVar::RefVar()
MPW-mangled: __ct__6RefVarFv
*/
    JT _ZN6RefVarC1Ev, 01802b84

/*
C++: TUPort::TUPort(unsigned long)
MPW-mangled: __ct__6TUPortFUl
*/
    JT _ZN6TUPortC1Em, 01800d98

/*
C++: CBuffer::CBuffer()
MPW-mangled: __ct__7CBufferFv
*/
    JT _ZN7CBufferC1Ev, 01801900

/*
C++: TAEvent::TAEvent()
MPW-mangled: __ct__7TAEventFv
*/
    JT _ZN7TAEventC2Ev, 01801904

/*
C++: TOption::TOption(unsigned long)
MPW-mangled: __ct__7TOptionFUl
*/
    JT _ZN7TOptionC1Em, 018001f4

/*
C++: DragBits::DragBits(TView*, const TRect*, unsigned char)
MPW-mangled: __ct__8DragBitsFP5TViewPC5TRectUc
*/
    JT _ZN8DragBitsC1EP5TViewPK5TRecth, 01800b54

/*
C++: TAddress::TAddress(unsigned char)
MPW-mangled: __ct__8TAddressFUc
*/
    JT _ZN8TAddressC1Eh, 01802078

/*
C++: TNBPName::TNBPName()
MPW-mangled: __ct__8TNBPNameFv
*/
    JT _ZN8TNBPNameC1Ev, 01802068

/*
C++: CNullPipe::CNullPipe(long)
MPW-mangled: __ct__9CNullPipeFl
*/
    JT _ZN9CNullPipeC1El, 01801908

/*
C++: CPartPipe::CPartPipe()
MPW-mangled: __ct__9CPartPipeFv
*/
    JT _ZN9CPartPipeC1Ev, 01800d9c

/*
C++: CRingPipe::CRingPipe()
MPW-mangled: __ct__9CRingPipeFv
*/
    JT _ZN9CRingPipeC1Ev, 0180190c

/*
C++: CTaskPipe::CTaskPipe()
MPW-mangled: __ct__9CTaskPipeFv
*/
    JT _ZN9CTaskPipeC1Ev, 01800da0

/*
C++: RefStruct::RefStruct(const long)
MPW-mangled: __ct__9RefStructFCl
*/
    JT _ZN9RefStructC1EKl, 01802b8c

/*
C++: RefStruct::RefStruct(const RefVar&)
MPW-mangled: __ct__9RefStructFRC6RefVar
*/
    JT _ZN9RefStructC1ERK6RefVar, 01802c74

/*
C++: RefStruct::RefStruct(const RefStruct&)
MPW-mangled: __ct__9RefStructFRC9RefStruct
*/
    JT _ZN9RefStructC1ERK9RefStruct, 01802c78

/*
C++: RefStruct::RefStruct()
MPW-mangled: __ct__9RefStructFv
*/
    JT _ZN9RefStructC1Ev, 01802b88

/*
C++: TAppWorld::TAppWorld()
MPW-mangled: __ct__9TAppWorldFv
*/
    JT _ZN9TAppWorldC1Ev, 01801910

/*
C++: TCommTool::TCommTool(unsigned long)
MPW-mangled: __ct__9TCommToolFUl
*/
    JT _ZN9TCommToolC1Em, 01800510

/*
C++: TCompiler::TCompiler(TInputStream*, int)
MPW-mangled: __ct__9TCompilerFP12TInputStreami
*/
    JT _ZN9TCompilerC1EP12TInputStreami, 018007c8

/*
C++: TUGestalt::TUGestalt()
MPW-mangled: __ct__9TUGestaltFv
*/
    JT _ZN9TUGestaltC1Ev, 01800da4

/*
C++: TUMonitor::TUMonitor(unsigned long)
MPW-mangled: __ct__9TUMonitorFUl
*/
    JT _ZN9TUMonitorC1Em, 01800da8

/*
C++: operator delete(void*)
MPW-mangled: __dl__FPv
*/
    JT _ZdlPv, 01800dac

/*
C++: CMemObject::~CMemObject()
MPW-mangled: __dt__10CMemObjectFv
*/
    DtorGlue _ZN10CMemObject, 01801914

/*
C++: CMinBuffer::~CMinBuffer()
MPW-mangled: __dt__10CMinBufferFv
*/
    DtorGlue _ZN10CMinBuffer, 01801918

/*
C++: TCircleBuf::~TCircleBuf()
MPW-mangled: __dt__10TCircleBufFv
*/
    DtorGlue _ZN10TCircleBuf, 01802b4c

/*
C++: TObjectPtr::~TObjectPtr()
MPW-mangled: __dt__10TObjectPtrFv
*/
    DtorGlue _ZN10TObjectPtr, 018007cc

/*
C++: TSoupIndex::~TSoupIndex()
MPW-mangled: __dt__10TSoupIndexFv
*/
    DtorGlue _ZN10TSoupIndex, 018007d0

/*
C++: TStartInfo::~TStartInfo()
MPW-mangled: __dt__10TStartInfoFv
*/
    DtorGlue _ZN10TStartInfo, 018020b4

/*
C++: CBufferList::~CBufferList()
MPW-mangled: __dt__11CBufferListFv
*/
    DtorGlue _ZN11CBufferList, 0180191c

/*
C++: CBufferPipe::~CBufferPipe()
MPW-mangled: __dt__11CBufferPipeFv
*/
    DtorGlue _ZN11CBufferPipe, 01801920

/*
C++: CRingBuffer::~CRingBuffer()
MPW-mangled: __dt__11CRingBufferFv
*/
    DtorGlue _ZN11CRingBuffer, 01801924

/*
C++: CSortedList::~CSortedList()
MPW-mangled: __dt__11CSortedListFv
*/
    DtorGlue _ZN11CSortedList, 01801928

/*
C++: TCardDevice::~TCardDevice()
MPW-mangled: __dt__11TCardDeviceFv
*/
    DtorGlue _ZN11TCardDevice, 01800db0

/*
C++: TCardPCMCIA::~TCardPCMCIA()
MPW-mangled: __dt__11TCardPCMCIAFv
*/
    DtorGlue _ZN11TCardPCMCIA, 01800db4

/*
C++: TCardSocket::~TCardSocket()
MPW-mangled: __dt__11TCardSocketFv
*/
    DtorGlue _ZN11TCardSocket, 01800db8

/*
C++: TGCSafeList::~TGCSafeList()
MPW-mangled: __dt__11TGCSafeListFv
*/
    DtorGlue _ZN11TGCSafeList, 0180192c

/*
C++: TPMIterator::~TPMIterator()
MPW-mangled: __dt__11TPMIteratorFv
*/
    DtorGlue _ZN11TPMIterator, 01800dbc

/*
C++: TUTaskWorld::~TUTaskWorld()
MPW-mangled: __dt__11TUTaskWorldFv
*/
    DtorGlue _ZN11TUTaskWorld, 01800dc0

/*
C++: TCardMessage::~TCardMessage()
MPW-mangled: __dt__12TCardMessageFv
*/
    DtorGlue _ZN12TCardMessage, 01800dc4

/*
C++: TCardPackage::~TCardPackage()
MPW-mangled: __dt__12TCardPackageFv
*/
    DtorGlue _ZN12TCardPackage, 01800dc8

/*
C++: TLookupNames::~TLookupNames()
MPW-mangled: __dt__12TLookupNamesFv
*/
    DtorGlue _ZN12TLookupNames, 01802074

/*
C++: TOptionArray::~TOptionArray()
MPW-mangled: __dt__12TOptionArrayFv
*/
    DtorGlue _ZN12TOptionArray, 018001f8

/*
C++: TPartHandler::~TPartHandler()
MPW-mangled: __dt__12TPartHandlerFv
*/
    DtorGlue _ZN12TPartHandler, 01800dcc

/*
C++: TUNameServer::~TUNameServer()
MPW-mangled: __dt__12TUNameServerFv
*/
    DtorGlue _ZN12TUNameServer, 01800dd0

/*
C++: CDynamicArray::~CDynamicArray()
MPW-mangled: __dt__13CDynamicArrayFv
*/
    DtorGlue _ZN13CDynamicArray, 01801930

/*
C++: TCardFunction::~TCardFunction()
MPW-mangled: __dt__13TCardFunctionFv
*/
    DtorGlue _ZN13TCardFunction, 01802dac

/*
C++: TCardLongLink::~TCardLongLink()
MPW-mangled: __dt__13TCardLongLinkFv
*/
    DtorGlue _ZN13TCardLongLink, 01800dd4

/*
C++: TEndpointPipe::~TEndpointPipe()
MPW-mangled: __dt__13TEndpointPipeFv
*/
    DtorGlue _ZN13TEndpointPipe, 018001fc

/*
C++: TMemoryObject::~TMemoryObject()
MPW-mangled: __dt__13TMemoryObjectFv
*/
    DtorGlue _ZN13TMemoryObject, 01802088

/*
C++: CArrayIterator::~CArrayIterator()
MPW-mangled: __dt__14CArrayIteratorFv
*/
    DtorGlue _ZN14CArrayIterator, 01801934

/*
C++: CBufferSegment::~CBufferSegment()
MPW-mangled: __dt__14CBufferSegmentFv
*/
    DtorGlue _ZN14CBufferSegment, 01801938

/*
C++: TAEventHandler::~TAEventHandler()
MPW-mangled: __dt__14TAEventHandlerFv
*/
    DtorGlue _ZN14TAEventHandler, 0180193c
    JT __dt__14TAEventHandlerFv, 0180193c   /* original function for glue code */

/*
C++: TCardATALoader::~TCardATALoader()
MPW-mangled: __dt__14TCardATALoaderFv
*/
    DtorGlue _ZN14TCardATALoader, 01802b0c

/*
C++: TPackageLoader::~TPackageLoader()
MPW-mangled: __dt__14TPackageLoaderFv
*/
    DtorGlue _ZN14TPackageLoader, 01800dd8

/*
C++: TUAsyncMessage::~TUAsyncMessage()
MPW-mangled: __dt__14TUAsyncMessageFv
*/
    DtorGlue _ZN14TUAsyncMessage, 01800ddc

/*
C++: TUSoundChannel::~TUSoundChannel()
MPW-mangled: __dt__14TUSoundChannelFv
*/
    DtorGlue _ZN14TUSoundChannel, 01802c64

/*
C++: CBaseRingBuffer::~CBaseRingBuffer()
MPW-mangled: __dt__15CBaseRingBufferFv
*/
    DtorGlue _ZN15CBaseRingBuffer, 01801940

/*
C++: TEndpointClient::~TEndpointClient()
MPW-mangled: __dt__15TEndpointClientFv
*/
    DtorGlue _ZN15TEndpointClient, 01800200

/*
C++: TEzEndpointPipe::~TEzEndpointPipe()
MPW-mangled: __dt__15TEzEndpointPipeFv
*/
    DtorGlue _ZN15TEzEndpointPipe, 01800204

/*
C++: TObjectIterator::~TObjectIterator()
MPW-mangled: __dt__15TObjectIteratorFv
*/
    DtorGlue _ZN15TObjectIterator, 018007d4

/*
C++: TOptionIterator::~TOptionIterator()
MPW-mangled: __dt__15TOptionIteratorFv
*/
    DtorGlue _ZN15TOptionIterator, 01800208

/*
C++: TPCMCIA20Parser::~TPCMCIA20Parser()
MPW-mangled: __dt__15TPCMCIA20ParserFv
*/
    DtorGlue _ZN15TPCMCIA20Parser, 01802584

/*
C++: TUDomainManager::~TUDomainManager()
MPW-mangled: __dt__15TUDomainManagerFv
*/
    DtorGlue _ZN15TUDomainManager, 01800de0

/*
C++: TCardCISIterator::~TCardCISIterator()
MPW-mangled: __dt__16TCardCISIteratorFv
*/
    DtorGlue _ZN16TCardCISIterator, 018024ec

/*
C++: TPackageIterator::~TPackageIterator()
MPW-mangled: __dt__16TPackageIteratorFv
*/
    DtorGlue _ZN16TPackageIterator, 01800de4

/*
C++: CShadowRingBuffer::~CShadowRingBuffer()
MPW-mangled: __dt__17CShadowRingBufferFv
*/
    DtorGlue _ZN17CShadowRingBuffer, 01801944

/*
C++: TATAPartitionInfo::~TATAPartitionInfo()
MPW-mangled: __dt__17TATAPartitionInfoFv
*/
    DtorGlue _ZN17TATAPartitionInfo, 01802518

/*
C++: TGCSafeSortedList::~TGCSafeSortedList()
MPW-mangled: __dt__17TGCSafeSortedListFv
*/
    DtorGlue _ZN17TGCSafeSortedList, 01801948

/*
C++: TLicenseeVAddress::~TLicenseeVAddress()
MPW-mangled: __dt__17TLicenseeVAddressFv
*/
    DtorGlue _ZN17TLicenseeVAddress, 01802558

/*
C++: TUserAbortHandler::~TUserAbortHandler()
MPW-mangled: __dt__17TUserAbortHandlerFv
*/
    DtorGlue _ZN17TUserAbortHandler, 0180020c

/*
C++: TATABootParamBlock::~TATABootParamBlock()
MPW-mangled: __dt__18TATABootParamBlockFv
*/
    DtorGlue _ZN18TATABootParamBlock, 01802520

/*
C++: TCardConfiguration::~TCardConfiguration()
MPW-mangled: __dt__18TCardConfigurationFv
*/
    DtorGlue _ZN18TCardConfiguration, 01800de8

/*
C++: TULockingSemaphore::~TULockingSemaphore()
MPW-mangled: __dt__18TULockingSemaphoreFv
*/
    DtorGlue _ZN18TULockingSemaphore, 01800dec

/*
C++: TUSoundCallbackProc::~TUSoundCallbackProc()
MPW-mangled: __dt__19TUSoundCallbackProcFv
*/
    DtorGlue _ZN19TUSoundCallbackProc, 01802c68

/*
C++: CShadowBufferSegment::~CShadowBufferSegment()
MPW-mangled: __dt__20CShadowBufferSegmentFv
*/
    DtorGlue _ZN20CShadowBufferSegment, 0180194c

/*
C++: TEndpointEventHandler::~TEndpointEventHandler()
MPW-mangled: __dt__21TEndpointEventHandlerFv
*/
    DtorGlue _ZN21TEndpointEventHandler, 01800210

/*
C++: CList::~CList()
MPW-mangled: __dt__5CListFv
*/
    DtorGlue _ZN5CList, 01801950

/*
C++: CPipe::~CPipe()
MPW-mangled: __dt__5CPipeFv
*/
    DtorGlue _ZN5CPipe, 01801954

/*
C++: TView::~TView()
MPW-mangled: __dt__5TViewFv
*/
    DtorGlue _ZN5TView, 01800b58

/*
C++: RefVar::~RefVar()
MPW-mangled: __dt__6RefVarFv
*/
    DtorGlue _ZN6RefVar, 01802054

/*
C++: CBuffer::~CBuffer()
MPW-mangled: __dt__7CBufferFv
*/
    DtorGlue _ZN7CBuffer, 01801958

/*
C++: DragBits::~DragBits()
MPW-mangled: __dt__8DragBitsFv
*/
    DtorGlue _ZN8DragBits, 01800b5c

/*
C++: TUObject::~TUObject()
MPW-mangled: __dt__8TUObjectFv
*/
    DtorGlue _ZN8TUObject, 01800df0

/*
C++: CNullPipe::~CNullPipe()
MPW-mangled: __dt__9CNullPipeFv
*/
    DtorGlue _ZN9CNullPipe, 0180195c

/*
C++: CPartPipe::~CPartPipe()
MPW-mangled: __dt__9CPartPipeFv
*/
    DtorGlue _ZN9CPartPipe, 01800df4

/*
C++: CRingPipe::~CRingPipe()
MPW-mangled: __dt__9CRingPipeFv
*/
    DtorGlue _ZN9CRingPipe, 01801960

/*
C++: CTaskPipe::~CTaskPipe()
MPW-mangled: __dt__9CTaskPipeFv
*/
    DtorGlue _ZN9CTaskPipe, 01800df8

/*
C++: RefStruct::~RefStruct()
MPW-mangled: __dt__9RefStructFv
*/
    DtorGlue _ZN9RefStruct, 01802c7c

/*
C++: TAppWorld::~TAppWorld()
MPW-mangled: __dt__9TAppWorldFv
*/
    DtorGlue _ZN9TAppWorld, 01801964

/*
C++: TCommTool::~TCommTool()
MPW-mangled: __dt__9TCommToolFv
*/
    DtorGlue _ZN9TCommTool, 01800514

/*
C++: TCompiler::~TCompiler()
MPW-mangled: __dt__9TCompilerFv
*/
    DtorGlue _ZN9TCompiler, 018007d8

/*
C++: TUMonitor::~TUMonitor()
MPW-mangled: __dt__9TUMonitorFv
*/
    DtorGlue _ZN9TUMonitor, 01800dfc

/*
C++: operator==(const TDate&, const TDate&)
MPW-mangled: __eq__FRC5TDateT1
*/
    JT __eq__FRC5TDateT1, 018025f8
    JT __fpclassifyd, 018028ac

/*
C++: operator>(const TDate&, const TDate&)
MPW-mangled: __gt__FRC5TDateT1
*/
    JT __gt__FRC5TDateT1, 018025fc
    JT __inf, 018028b0
    JT __isfinited, 018028b4
    JT __isnand, 018028b8
    JT __isnanf, 018028bc
    JT __isnormald, 018028c0

/*
C++: CPipe::operator<<(signed char)
MPW-mangled: __ls__5CPipeFSc
*/
    JT _ZN5CPipelsEa, 01801974

/*
C++: CPipe::operator<<(unsigned char)
MPW-mangled: __ls__5CPipeFUc
*/
    JT _ZN5CPipelsEh, 01801978

/*
C++: CPipe::operator<<(unsigned long)
MPW-mangled: __ls__5CPipeFUl
*/
    JT _ZN5CPipelsEm, 0180197c

/*
C++: CPipe::operator<<(unsigned short)
MPW-mangled: __ls__5CPipeFUs
*/
    JT _ZN5CPipelsEt, 01801980

/*
C++: CPipe::operator<<(char)
MPW-mangled: __ls__5CPipeFc
*/
    JT _ZN5CPipelsEc, 01801968

/*
C++: CPipe::operator<<(long)
MPW-mangled: __ls__5CPipeFl
*/
    JT _ZN5CPipelsEl, 0180196c

/*
C++: CPipe::operator<<(short)
MPW-mangled: __ls__5CPipeFs
*/
    JT _ZN5CPipelsEs, 01801970

/*
C++: operator<(const TDate&, const TDate&)
MPW-mangled: __lt__FRC5TDateT1
*/
    JT __lt__FRC5TDateT1, 01802600
    JT __multiply, 01800000

/*
C++: operator new(unsigned int)
MPW-mangled: __nw__FUi
*/
    JT _Znwj, 01800e04

/*
C++: operator new(unsigned int, void*)
MPW-mangled: __nw__FUiPv
*/
    JT _ZnwjPv, 01800044

/*
C++: TObjectPtr::operator char*() const
MPW-mangled: __opPc__10TObjectPtrCFv
*/
    JT __opPc__10TObjectPtrCFv, 018007dc

/*
C++: DataPtr::operator char*() const
MPW-mangled: __opPc__7DataPtrCFv
*/
    JT __opPc__7DataPtrCFv, 018007e0

/*
C++: RefVar::operator long() const
MPW-mangled: __opl__6RefVarCFv
*/
    JT __opl__6RefVarCFv, 01802c84

/*
C++: RefStruct::operator long() const
MPW-mangled: __opl__9RefStructCFv
*/
    JT __opl__9RefStructCFv, 01802c88
    JT __pure_virtual_called, 01800e08
    JT __remainder, 01800004

/*
C++: CPipe::operator>>(signed char&)
MPW-mangled: __rs__5CPipeFRSc
*/
    JT _ZN5CPipersERa, 01801990

/*
C++: CPipe::operator>>(unsigned char&)
MPW-mangled: __rs__5CPipeFRUc
*/
    JT _ZN5CPipersERh, 01801994

/*
C++: CPipe::operator>>(unsigned long&)
MPW-mangled: __rs__5CPipeFRUl
*/
    JT _ZN5CPipersERm, 01801998

/*
C++: CPipe::operator>>(unsigned short&)
MPW-mangled: __rs__5CPipeFRUs
*/
    JT _ZN5CPipersERt, 0180199c

/*
C++: CPipe::operator>>(char&)
MPW-mangled: __rs__5CPipeFRc
*/
    JT _ZN5CPipersERc, 01801984

/*
C++: CPipe::operator>>(long&)
MPW-mangled: __rs__5CPipeFRl
*/
    JT _ZN5CPipersERl, 01801988

/*
C++: CPipe::operator>>(short&)
MPW-mangled: __rs__5CPipeFRs
*/
    JT _ZN5CPipersERs, 0180198c
    JT __rt_divtest, 01800008
    JT __rt_sdiv, 0180000c
    JT __rt_sdiv10, 01800010
    JT __rt_udiv, 01800014
    JT __rt_udiv10, 01800018
    JT __signbitd, 018028c4

/*
C++: TSuppress::__st
MPW-mangled: __st__9TSuppress
*/
/*
    Function is empty in NewtonInterfaceLib.
*/

    JT __udiv10, 0180001c
    JT __uremainder, 01800020

/*
C++: __vec_delete(void*, int, int, void*, int, int)
MPW-mangled: __vec_delete__FPviT2T1N22
*/
    JT __vec_delete__FPviT2T1N22, 01800e0c

/*
C++: __vec_new(void*, int, int, void*)
MPW-mangled: __vec_new__FPviT2T1
*/
    JT __vec_new__FPviT2T1, 01800e10
    JT _sprintf, 01800048
    JT _vsprintf, 0180004c
    JT acos, 01800050
    JT acosh, 01800054
    JT annuity, 01800058
    JT asin, 0180005c
    JT asinh, 01800060
    JT atan, 01800064
    JT atan2, 01800068
    JT atanh, 0180006c
    JT atof, 01800070
    JT atoi, 01800074
    JT atol, 01800078
    JT bsearch, 0180007c
    JT ceil, 01800080
    JT compound, 01800084
    JT copysign, 01800088
    JT cos, 0180008c
    JT cosh, 01800090
    JT erf, 01800094
    JT erfc, 01800098
    JT exp, 0180009c
    JT exp2, 018000a0
    JT expm1, 018000a4
    JT fabs, 018000a8
    JT fclose, 01801f20
    JT fdim, 018000ac
    JT feclearexcept, 018000b0
    JT fegetenv, 018000b4
    JT fegetexcept, 018000b8
    JT fegetround, 018000bc
    JT feholdexcept, 018000c0
    JT feof, 01801f24
    JT feraiseexcept, 018000c4
    JT ferror, 01801f28
    JT fesetenv, 018000c8
    JT fesetexcept, 018000cc
    JT fesetround, 018000d0
    JT fetestexcept, 018000d4
    JT feupdateenv, 018000d8
    JT fflush, 01801f2c
    JT floor, 018000dc
    JT fmax, 018000e0
    JT fmin, 018000e4
    JT fmod, 018000e8
    JT fopen, 01801f30
    JT fprintf, 01801f34
    JT fputc, 01801f38
    JT fputs, 01801f3c
    JT fread, 01801f40
    JT free, 01801024
    JT frexp, 018000ec
    JT fwrite, 01801f44
    JT gamma, 018000f0
    JT get_rand_state, 01802b9c
    JT getc, 01801f48
    JT hypot, 018000f4
    JT ldexp, 018000f8
    JT lgamma, 018000fc
    JT log, 01800100
    JT log10, 01800104
    JT log1p, 01800108
    JT log2, 0180010c
    JT logb, 01800110
    JT longjmp, 0180003c
    JT malloc, 01801234
    JT memchr, 01800114
    JT memcmp, 01800118
    JT memcpy, 0180011c
    JT memmove, 01800120
    JT memset, 01800124
    JT modf, 018028c8

/*
C++: TEndpoint::nAbort(unsigned char)
MPW-mangled: nAbort__9TEndpointFUc
*/
    JT _ZN9TEndpoint6nAbortEh, 018025e0

/*
C++: TEndpoint::nRcv(unsigned char*, long*, long, unsigned long*, unsigned long, unsigned char, TOptionArray*)
MPW-mangled: nRcv__9TEndpointFPUcPllPUlUlUcP12TOptionArray
*/
    JT _ZN9TEndpoint4nRcvEPhPllPmmhP12TOptionArray, 018025e4
    JT nan, 018028cc
    JT nanf, 018028d0
    JT nanl, 018028d4
    JT nearbyint, 01800128
    JT nextafterd, 0180012c
    JT pow, 01800130
    JT printf, 01801f4c
    JT putc, 01801f50
    JT qsort, 018028d8
    JT rand, 01800134
    JT randomx, 01800138
    JT realloc, 018020ec

/*
C++: realloc(void*, unsigned int)
MPW-mangled: realloc__FPvUi
*/
    JT _Z7reallocPvj, 018012fc
    JT relation, 0180013c
    JT remainder, 01800140
    JT remquo, 01800144
    JT rint, 01800148
    JT rinttol, 0180014c
    JT round, 01800150
    JT roundtol, 018028dc
    JT scalb, 01800154
    JT set_rand_state, 01802ba0
    JT setjmp, 01800040
    JT setvbuf, 01801f54
    JT sin, 01800158
    JT sinh, 0180015c
    JT sizeof_rand_state, 01802ba4
    JT sprintf, 01800160
    JT sqrt, 01800164
    JT srand, 01800168
    JT sscanf, 0180016c
    JT strcat, 01800170
    JT strchr, 018028e0
    JT strcmp, 01800174
    JT strcoll, 018028e4
    JT strcpy, 01800178
    JT strcspn, 018028e8
    JT strlen, 0180017c
    JT strncat, 01800180
    JT strncmp, 01800184
    JT strncpy, 01800188
    JT strpbrk, 018028ec
    JT strrchr, 018028f0
    JT strspn, 018028f4
    JT strstr, 0180018c
    JT strtod, 01800190
    JT strtok, 018028f8
    JT strtol, 01800194
    JT strtoul, 018028fc
    JT strxfrm, 01802900
    JT symcmp__FPcT1, 01802b2c
    JT tan, 01800198
    JT tanh, 0180019c
    JT tolower, 018001a0
    JT toupper, 018001a4
    JT trunc, 018001a8
    JT ungetc, 01801f58
    JT vfprintf, 01801f5c
    JT vsprintf, 018001ac

