// ==============================
// File:			TToolkitPrototypes.cp
// Project:			Einstein
//
// Copyright 2003-2022 by Paul Guyot and Matthias Melcher.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

#include "TToolkitPrototypes.h"

const char* TToolkitPrototype::NewtonDefs21 =
	/*
	 Copyright:    (c) 1993-1997 by Apple Computer, Inc., all rights reserved.
	 "Newton 2.1" Platform File Version 1.2b2 Constant Definitions
	 Generated on 5/2/97 at 11:43:23 AM PST
	 */
	"constant platformVersion :=\n"
	"{\n"
	"    platformFile: |Newton 2.1|,\n"
	"    version:      |1.2b2|,\n"
	"    buildDate:    |5/2/97|,\n"
	"    buildTime:    |11:43:23 AM PST|,\n"
	"    timeOfBuild:  49090320\n"
	"};\n"
	"constant protoPeriodicAlarmEditor := @2;\n"
	"constant ROM_alarmWakeup := @4;\n"
	"constant protoAnalogClock := @5;\n"
	"constant ROM_checkBitmaps := @11;\n"
	"constant ROM_priorityItems := @12;\n"
	"constant ROM_bootSound := @16;\n"
	"constant protoRepeatDateDurationTextPicker := @19;\n"
	"constant protoSenderPopup := @20;\n"
	"constant protoStatusBarber := @21;\n"
	"constant ROM_calendarNotesName := @22;\n"
	"constant ROM_calendarSoupName := @23;\n"
	"constant newtAreaCodeLine := @26;\n"
	"constant protoCanonicalCompass := @28;\n"
	"constant ROM_cardfileSoupName := @47;\n"
	"constant ROM_click := @51;\n"
	"constant protoFrameFormat := @52;\n"
	"constant ROM_cloud1 := @53;\n"
	"constant ROM_cloud2 := @54;\n"
	"constant ROM_cloud3 := @55;\n"
	"constant protoCoverPageFormatDeprecated := @60;\n"
	"constant ROM_crumple := @62;\n"
	"constant ROM_dataName := @63;\n"
	"constant ROM_dateTimeStrSpecs := @66;\n"
	"constant ROM_dialtones := @68;\n"
	"constant protoNumberPicker := @72;\n"
	"constant newtStatusBarNoClose := @73;\n"
	"constant protoSmartCluster := @75;\n"
	"constant ROM_drawerClose := @76;\n"
	"constant ROM_drawerOpen := @77;\n"
	"constant ROM_errorTable := @79;\n"
	"constant ROM_flip := @85;\n"
	"constant ROM_fontSystem10 := @87;\n"
	"constant ROM_fontSystem10Bold := @88;\n"
	"constant ROM_fontSystem10Underline := @89;\n"
	"constant ROM_fontSystem12 := @90;\n"
	"constant ROM_fontSystem12Bold := @91;\n"
	"constant ROM_fontSystem12Underline := @92;\n"
	"constant ROM_fontSystem14 := @93;\n"
	"constant ROM_fontSystem14Bold := @94;\n"
	"constant ROM_fontSystem14Underline := @95;\n"
	"constant ROM_fontSystem18 := @96;\n"
	"constant ROM_fontSystem18Bold := @97;\n"
	"constant ROM_fontSystem18Underline := @98;\n"
	"constant ROM_fontSystem9 := @99;\n"
	"constant ROM_fontSystem9Bold := @100;\n"
	"constant ROM_fontSystem9Underline := @101;\n"
	"constant ROM_funBeep := @102;\n"
	"constant newtPrefsView := @107;\n"
	"constant protoYearPicker := @108;\n"
	"constant ROM_hiliteSound := @110;\n"
	"constant ROM_inboxSoupName := @111;\n"
	"constant protoRepeatPicker := @123;\n"
	"constant newtInfoButton := @124;\n"
	"constant protoTimeButton := @128;\n"
	"constant ROM_metaSoupName := @129;\n"
	"constant protoDragger := @132;\n"
	"constant protoNotesContent := @134;\n"
	"constant ROM_outboxSoupName := @139;\n"
	"constant newtFolderTab := @143;\n"
	"constant ROM_paperRollSoupName := @144;\n"
	"constant newtCountrySymbolFilter := @146;\n"
	"constant protoTimePopup := @147;\n"
	"constant ROM_phoneText := @148;\n"
	"constant ROM_plinkBeep := @150;\n"
	"constant newtAboutView := @152;\n"
	"constant protoPrinterChooserButton := @153;\n"
	"constant protoApp := @157;\n"
	"constant protoBorder := @160;\n"
	"constant newtClockFolderTab := @162;\n"
	"constant protoLargeClosebox := @163;\n"
	"constant protoCheckbox := @164;\n"
	"constant protoCheckboxIcon := @165;\n"
	"constant protoClosebox := @166;\n"
	"constant protoDateExpandoDeprecated := @170;\n"
	"constant protoDivider := @172;\n"
	"constant protoDrawer := @173;\n"
	"constant protoExpandoShellDeprecated := @175;\n"
	"constant protoFilingButton := @176;\n"
	"constant protoFloater := @179;\n"
	"constant protoFloatNGo := @180;\n"
	"constant protoGauge := @182;\n"
	"constant protoGlance := @183;\n"
	"constant protoInputLine := @185;\n"
	"constant protoKeyboard := @187;\n"
	"constant protoKeypad := @188;\n"
	"constant protoLabelInputLine := @189;\n"
	"constant protoLabelPicker := @190;\n"
	"constant protoOverview := @191;\n"
	"constant protoPhoneExpandoDeprecated := @194;\n"
	"constant protoPicker := @195;\n"
	"constant protoPictIndexer := @196;\n"
	"constant protoPictRadioButton := @197;\n"
	"constant protoPictureButton := @198;\n"
	"constant protoPrintFormat := @200;\n"
	"constant protoRadioButton := @202;\n"
	"constant protoRadioCluster := @203;\n"
	"constant protoRCheckbox := @204;\n"
	"constant protoRoll := @206;\n"
	"constant protoRollBrowser := @207;\n"
	"constant protoRollItem := @208;\n"
	"constant protoActionButton := @209;\n"
	"constant protoSetClock := @210;\n"
	"constant protoFolderTab := @211;\n"
	"constant protoSlider := @212;\n"
	"constant protoStaticText := @218;\n"
	"constant protoStatus := @219;\n"
	"constant protoStatusBar := @220;\n"
	"constant protoTable := @223;\n"
	"constant protoTableDef := @224;\n"
	"constant protoTableEntry := @225;\n"
	"constant protoTextButton := @226;\n"
	"constant protoTextExpandoDeprecated := @227;\n"
	"constant protoTextList := @228;\n"
	"constant protoTitle := @229;\n"
	"constant protoPeopleDataDef := @231;\n"
	"constant protoRecToggle := @234;\n"
	"constant protoStdClosing := @236;\n"
	"constant ROM_soupFinder := @237;\n"
	"constant ROM_repeatMeetingName := @239;\n"
	"constant protoNameRefDataDef := @240;\n"
	"constant ROM_nameRefValidationFrame := @241;\n"
	"constant ROM_repeatNotesName := @242;\n"
	"constant ROM_citySoupName := @251;\n"
	"constant ROM_radioOnBitmap := @257;\n"
	"constant protoAddressPicker := @259;\n"
	"constant protoRoutingFormat := @260;\n"
	"constant ROM_shapeName := @261;\n"
	"constant ROM_simpleBeep := @262;\n"
	"constant ROM_soundOff := @263;\n"
	"constant ROM_systemSoupName := @276;\n"
	"constant protoRepeatView := @279;\n"
	"constant protoAnalogTimePopup := @280;\n"
	"constant ROM_todoSoupName := @282;\n"
	"constant protoTimeIntervalPopup := @284;\n"
	"constant protoDateNTimePopup := @288;\n"
	"constant ROM_wakeupBeep := @289;\n"
	"constant ROM_routeDeleteIcon := @291;\n"
	"constant ROM_routeDuplicateIcon := @292;\n"
	"constant newtAreaCodePhoneLine := @294;\n"
	"constant ROM_countrySoupName := @295;\n"
	"constant ROM_radioOffBitmap := @297;\n"
	"constant ROM_tickSound := @299;\n"
	"constant ROM_tockSound := @300;\n"
	"constant ROM_cuckooSound := @301;\n"
	"constant ROM_ratchetSound := @302;\n"
	"constant ROM_addSound := @303;\n"
	"constant ROM_removeSound := @304;\n"
	"constant protoLetterFormat := @305;\n"
	"constant newtEntryPageHeader := @309;\n"
	"constant ROM_cancelBitmap := @311;\n"
	"constant ROM_plunk := @313;\n"
	"constant ROM_poof := @314;\n"
	"constant protoLabeledBatteryGauge := @316;\n"
	"constant protoDatePopup := @317;\n"
	"constant ROM_worldMapBitmap := @321;\n"
	"constant ROM_phoneBitmap := @322;\n"
	"constant ROM_upBitmap := @323;\n"
	"constant ROM_downBitmap := @324;\n"
	"constant ROM_leftBitmap := @325;\n"
	"constant ROM_rightBitmap := @326;\n"
	"constant ROM_upArrowBitmap := @327;\n"
	"constant ROM_downArrowBitmap := @328;\n"
	"constant ROM_overviewBitmap := @329;\n"
	"constant ROM_keyCapsBitmap := @331;\n"
	"constant ROM_keyButtBitmap := @332;\n"
	"constant ROM_goAwayBitmap := @334;\n"
	"constant protoAZVertTabs := @339;\n"
	"constant ROM_AZTabsMaskA := @340;\n"
	"constant ROM_markupBitmap := @342;\n"
	"constant ROM_noMarkupBitmap := @343;\n"
	"constant ROM_bookmarkBitmap := @344;\n"
	"constant ROM_cardBitmap := @347;\n"
	"constant ROM_a2zBitmap := @349;\n"
	"constant ROM_zero2nineBitmap := @350;\n"
	"constant ROM_bookBitmap := @351;\n"
	"constant protoDateIntervalPopup := @356;\n"
	"constant protoMultiDatePopup := @357;\n"
	"constant protoYearPopup := @358;\n"
	"constant ROM_compatibleFinder := @359;\n"
	"constant protoMapPopup := @360;\n"
	"constant protoPeoplePopup := @371;\n"
	"constant newtRolloverLayout := @374;\n"
	"constant protoAlphaKeyboard := @375;\n"
	"constant protoNumericKeyboard := @376;\n"
	"constant protoPopinPlace := @377;\n"
	"constant protoNewSetClock := @378;\n"
	"constant protoAMPMCluster := @379;\n"
	"constant ROM_copier := @380;\n"
	"constant ROM_infoBitmap := @381;\n"
	"constant protoBasicEndpoint := @383;\n"
	"constant protoPrefsRollItem := @385;\n"
	"constant protoPopupButton := @386;\n"
	"constant protoDatePicker := @387;\n"
	"constant protoTransport := @389;\n"
	"constant protoCharEdit := @393;\n"
	"constant ROM_AZTabsBitmap := @396;\n"
	"constant ROM_AZTabsMaskCZ := @397;\n"
	"constant newtApplication := @398;\n"
	"constant newtStatusBar := @401;\n"
	"constant newtLayout := @402;\n"
	"constant newtRollLayout := @403;\n"
	"constant newtPageLayout := @404;\n"
	"constant newtOverLayout := @405;\n"
	"constant newtEntryView := @406;\n"
	"constant newtEntryViewActionButton := @407;\n"
	"constant newtEntryViewFilingButton := @408;\n"
	"constant newtRollEntryView := @409;\n"
	"constant newtEntryRollHeader := @410;\n"
	"constant newtStationeryView := @411;\n"
	"constant newtROEditView := @412;\n"
	"constant newtEditView := @413;\n"
	"constant newtROTextView := @414;\n"
	"constant newtTextView := @415;\n"
	"constant newtRONumView := @416;\n"
	"constant newtNumView := @417;\n"
	"constant newtROTextDateView := @418;\n"
	"constant newtTextDateView := @419;\n"
	"constant newtQBETextView := @420;\n"
	"constant newtROLabelInputLine := @421;\n"
	"constant newtLabelInputLine := @422;\n"
	"constant newtLabelNumInputLine := @423;\n"
	"constant newtLabelDateInputLine := @424;\n"
	"constant newtLabelPhoneInputLine := @425;\n"
	"constant newtQBELabelInputLine := @426;\n"
	"constant newtSmartNameView := @427;\n"
	"constant newtSmartPhoneView := @428;\n"
	"constant newtSoup := @429;\n"
	"constant newtAZTabs := @430;\n"
	"constant protoSoundChannel := @431;\n"
	"constant ROM_typewriter := @432;\n"
	"constant protoKeyboardButton := @434;\n"
	"constant protoRubricPopup := @435;\n"
	"constant protoLocationPopup := @436;\n"
	"constant newtCustomFilter := @437;\n"
	"constant newtLabelCustomInputLine := @438;\n"
	"constant newtActionButton := @439;\n"
	"constant newtFilingButton := @440;\n"
	"constant ROM_rcNoRecog := @442;\n"
	"constant ROM_rcPrefsConfig := @443;\n"
	"constant ROM_rcDefaultConfig := @444;\n"
	"constant ROM_rcSingleCharacterConfig := @445;\n"
	"constant ROM_rcTryLettersConfig := @446;\n"
	"constant ROM_rcRerecognizeConfig := @447;\n"
	"constant ROM_rcBuildChains := @448;\n"
	"constant ROM_rcInkOrText := @449;\n"
	"constant protoRecConfig := @450;\n"
	"constant newtStationery := @451;\n"
	"constant ROM_USStateSoupName := @452;\n"
	"constant ROM_canadianProvinceName := @453;\n"
	"constant ROM_continents := @454;\n"
	"constant protoWorldPicker := @455;\n"
	"constant protoStatePicker := @456;\n"
	"constant protoProvincePicker := @457;\n"
	"constant protoCountryPicker := @458;\n"
	"constant newtFloatingBar := @459;\n"
	"constant protoSoupOverview := @460;\n"
	"constant protoListPicker := @461;\n"
	"constant protoDigit := @462;\n"
	"constant protoDigitalClock := @463;\n"
	"constant protoStreamingEndpoint := @466;\n"
	"constant protoStatusTemplate := @467;\n"
	"constant protoStatusClosebox := @468;\n"
	"constant protoStatusIcon := @469;\n"
	"constant protoStatusButton := @470;\n"
	"constant protoStatusGauge := @471;\n"
	"constant protoStatusProgress := @472;\n"
	"constant protoStatusText := @473;\n"
	"constant protoOrientation := @474;\n"
	"constant protoHorizontalUpDownScroller := @475;\n"
	"constant protoTransportHeader := @477;\n"
	"constant protoInfoButton := @478;\n"
	"constant protoThumbnailFloater := @481;\n"
	"constant protoThumbnail := @484;\n"
	"constant protoImageView := @485;\n"
	"constant protoFormatPicker := @487;\n"
	"constant protoSendButton := @488;\n"
	"constant newtROTextTimeView := @491;\n"
	"constant newtTextTimeView := @492;\n"
	"constant newtLabelTimeInputLine := @493;\n"
	"constant newtROSymbolView := @494;\n"
	"constant newtSymbolView := @495;\n"
	"constant newtLabelSymbolInputLine := @496;\n"
	"constant protoPersonaPopup := @497;\n"
	"constant protoEmporiumPopup := @498;\n"
	"constant protoDisplayClock := @504;\n"
	"constant newtEntryLockedIcon := @507;\n"
	"constant protoLocationPicker := @512;\n"
	"constant ROM_AustralianStateName := @513;\n"
	"constant protoDigitalTimePopup := @514;\n"
	"constant ROM_faxRoutingIcon := @515;\n"
	"constant protoTimeDeltaPicker := @516;\n"
	"constant protoTimeDeltaPopup := @517;\n"
	"constant protoLatitudePicker := @518;\n"
	"constant protoLongitudePicker := @519;\n"
	"constant protoLongLatPicker := @521;\n"
	"constant protoTimeDeltaTextPicker := @522;\n"
	"constant protoLongLatTextPicker := @523;\n"
	"constant ROM_UKCountyName := @524;\n"
	"constant protoAliasCursor := @536;\n"
	"constant ROM_defaultInfoPrefs := @551;\n"
	"constant protoBlindEntryLine := @552;\n"
	"constant protoCountryLocPicker := @553;\n"
	"constant protoStateLocPicker := @554;\n"
	"constant protoDateNYearPicker := @555;\n"
	"constant protoMultiLinePicker := @559;\n"
	"constant ROM_AZTabsSlimBitmap := @560;\n"
	"constant ROM_AZTabsSlimMaskCZ := @561;\n"
	"constant ROM_AZTabsSlimMaskA := @562;\n"
	"constant protoTitleText := @563;\n"
	"constant newtProtoLineBase := @564;\n"
	"constant newtProtoLine := @565;\n"
	"constant newtPhonePopupEdit := @587;\n"
	"constant ROM_DefRotateFunc := @588;\n"
	"constant ROM_alarmIconTinyBitmap := @591;\n"
	"constant ROM_helpbookBitmap := @593;\n"
	"constant ROM_diamanteBitmap := @594;\n"
	"constant ROM_openPadlockBitmap := @600;\n"
	"constant ROM_closedPadlockBitmap := @601;\n"
	"constant ROM_colonSlides := @603;\n"
	"constant ROM_ampmSlides := @604;\n"
	"constant ROM_canonicalCharGrid := @606;\n"
	"constant protoDateDurationTextPicker := @607;\n"
	"constant protoHorizontal2DScroller := @608;\n"
	"constant protoLogPicker := @609;\n"
	"constant protoCorrectInfo := @614;\n"
	"constant protoWordInfo := @615;\n"
	"constant protoWordInterp := @616;\n"
	"constant protoAZTabs := @617;\n"
	"constant newtCheckBox := @618;\n"
	"constant newtROLabelNumInputLine := @619;\n"
	"constant newtROLabelDateInputLine := @620;\n"
	"constant newtROLabelTimeInputLine := @621;\n"
	"constant ROM_digitSlides := @622;\n"
	"constant protoDesktopEndpoint := @623;\n"
	"constant protoSmallKeyboardButton := @624;\n"
	"constant protoDiamondButton := @625;\n"
	"constant protoTextPicker := @626;\n"
	"constant protoTimeTextPicker := @627;\n"
	"constant protoDurationTextPicker := @628;\n"
	"constant protoDateTextPicker := @629;\n"
	"constant protoDateNTimeTextPicker := @630;\n"
	"constant protoMapTextPicker := @631;\n"
	"constant protoCountryTextPicker := @632;\n"
	"constant protoUSStatesTextPicker := @633;\n"
	"constant protoCitiesTextPicker := @634;\n"
	"constant newtNRLabelTimeInputLine := @635;\n"
	"constant newtNRLabelDateInputLine := @636;\n"
	"constant newtNRLabelDateNTimeInputLine := @637;\n"
	"constant newtFilter := @638;\n"
	"constant newtTextFilter := @639;\n"
	"constant newtIntegerFilter := @640;\n"
	"constant newtNumberFilter := @641;\n"
	"constant newtDateFilter := @642;\n"
	"constant newtTimeFilter := @643;\n"
	"constant newtDateNTimeFilter := @644;\n"
	"constant newtSymbolFilter := @645;\n"
	"constant newtPhoneFilter := @646;\n"
	"constant newtCityFilter := @647;\n"
	"constant newtStateFilter := @648;\n"
	"constant newtCountryFilter := @649;\n"
	"constant newtSmartNameFilter := @650;\n"
	"constant ROM_flapSlides := @651;\n"
	"constant newtFalseEntryView := @652;\n"
	"constant protoFullRouteSlip := @655;\n"
	"constant protoUpDownScroller := @656;\n"
	"constant protoLeftRightScroller := @657;\n"
	"constant ROM_dstSoupName := @658;\n"
	"constant newtSmartCompanyFilter := @659;\n"
	"constant newtSmartAddressFilter := @660;\n"
	"constant protoPeoplePicker := @664;\n"
	"constant protoNewFolderTab := @669;\n"
	"constant protoClockFolderTab := @670;\n"
	"constant protoGeneralPopup := @671;\n"
	"constant newtInfoBox := @672;\n"
	"constant protoRichInputLine := @674;\n"
	"constant protoRichLabelInputLine := @675;\n"
	"constant protoPhonePad := @676;\n"
	"constant protoTouchTonePad := @677;\n"
	"constant protoTransportPrefs := @678;\n"
	"constant ROM_canonicalBaseInfo := @681;\n"
	"constant newtLabelSimpleDateInputLine := @682;\n"
	"constant newtSimpleDateFilter := @683;\n"
	"constant ROM_digitFlapLowOrd := @685;\n"
	"constant ROM_digitFlap := @686;\n"
	"constant ROM_digitWideFlap := @687;\n"
	"constant ROM_plusMinusBitmap := @688;\n"
	"constant ROM_northSouthBitmap := @689;\n"
	"constant ROM_eastWestBitmap := @690;\n"
	"constant ROM_draggeurBitmap := @691;\n"
	"constant ROM_draggeurNoGrayBitmap := @692;\n"
	"constant ROM_dtmf0Bitmap := @693;\n"
	"constant ROM_dtmf1Bitmap := @694;\n"
	"constant ROM_dtmf2Bitmap := @695;\n"
	"constant ROM_dtmf3Bitmap := @696;\n"
	"constant ROM_dtmf4Bitmap := @697;\n"
	"constant ROM_dtmf5Bitmap := @698;\n"
	"constant ROM_dtmf6Bitmap := @699;\n"
	"constant ROM_dtmf7Bitmap := @700;\n"
	"constant ROM_dtmf8Bitmap := @701;\n"
	"constant ROM_dtmf9Bitmap := @702;\n"
	"constant ROM_dtmfStarBitmap := @703;\n"
	"constant ROM_dtmfPoundBitmap := @704;\n"
	"constant ROM_finePenTipBitmap := @707;\n"
	"constant ROM_smallPenTipBitmap := @708;\n"
	"constant ROM_mediumPenTipBitmap := @709;\n"
	"constant ROM_largePenTipBitmap := @710;\n"
	"constant ROM_handScrollBitmap := @713;\n"
	"constant ROM_keyButtonBitmap := @714;\n"
	"constant ROM_keySmallButtonBitmap := @715;\n"
	"constant ROM_keyPadBitmap := @716;\n"
	"constant ROM_prefsBitmap := @719;\n"
	"constant ROM_routePrintIcon := @730;\n"
	"constant ROM_routeBeamIcon := @731;\n"
	"constant ROM_routeMailIcon := @732;\n"
	"constant ROM_keyBulletBitmap := @734;\n"
	"constant ROM_keyColonBitmap := @735;\n"
	"constant ROM_keyCommaBitmap := @736;\n"
	"constant ROM_keyDivideBitmap := @737;\n"
	"constant ROM_keyEqualsBitmap := @738;\n"
	"constant ROM_keyLeftParenBitmap := @739;\n"
	"constant ROM_keyMinusBitmap := @740;\n"
	"constant ROM_keyPeriodBitmap := @741;\n"
	"constant ROM_keyPlusBitmap := @742;\n"
	"constant ROM_keyRadicalBitmap := @743;\n"
	"constant ROM_keyRightParenBitmap := @744;\n"
	"constant ROM_keySlashBitmap := @745;\n"
	"constant ROM_keyTimesBitmap := @746;\n"
	"constant ROM_kbdReturnBitmap := @747;\n"
	"constant ROM_kbdTabBitmap := @748;\n"
	"constant ROM_kbdLeftBitmap := @749;\n"
	"constant ROM_kbdRightBitmap := @750;\n"
	"constant ROM_kbdDictBitmap := @751;\n"
	"constant ROM_kbdDeleteBitmap := @752;\n"
	"constant ROM_kbdShiftBitmap := @753;\n"
	"constant ROM_kbdOptionBitmap := @754;\n"
	"constant ROM_kbdCapslockBitmap := @755;\n"
	"constant ROM_routeReply := @756;\n"
	"constant ROM_routeReaddress := @757;\n"
	"constant ROM_routePutAway := @758;\n"
	"constant ROM_routeLog := @759;\n"
	"constant ROM_routeCallBitmap := @760;\n"
	"constant ROM_routeUpdateBitmap := @761;\n"
	"constant ROM_checkBitmap := @763;\n"
	"constant ROM_notesBitmap := @765;\n"
	"constant ROM_topicMarkers := @766;\n"
	"constant ROM_routeFaxIcon := @768;\n"
	"constant ROM_routeTransport := @769;\n"
	"constant ROM_routeMissing := @770;\n"
	"constant ROM_routeForward := @771;\n"
	"constant ROM_routeAddSender := @772;\n"
	"constant ROM_routePasteText := @773;\n"
	"constant ROM_transportScripts := @774;\n"
	"constant protoDragNGo := @804;\n"
	"constant newtStationeryPopupButton := @812;\n"
	"constant newtNewStationeryButton := @813;\n"
	"constant newtShowStationeryButton := @814;\n"
	"constant newtRollShowStationeryButton := @815;\n"
	"constant newtEntryShowStationeryButton := @816;\n"
	"constant protoTXView := @826;\n"
	"constant protoTXViewFinder := @827;\n"
	"constant ROM_GetSerialNumber := @846;\n"
	"constant protoSoundFrame := @849;\n"
	"constant protoRecorderView := @853;\n"
	"constant newtCheckAllButton := @872;\n"
	"constant vjLeftH := 0;\n"
	"constant vjRightH := 1;\n"
	"constant vjCenterH := 2;\n"
	"constant vjFullH := 3;\n"
	"constant vjHMask := 3;\n"
	"constant vjTopV := 0*4;\n"
	"constant vjCenterV := 1*4;\n"
	"constant vjBottomV := 2*4;\n"
	"constant vjFullV := 3*4;\n"
	"constant vjVMask := 3*4;\n"
	"constant vjParentLeftH := 0*16;\n"
	"constant vjParentCenterH := 1*16;\n"
	"constant vjParentRightH := 2*16;\n"
	"constant vjParentFullH := 3*16;\n"
	"constant vjParentHMask := 3*16;\n"
	"constant vjParentTopV := 0*64;\n"
	"constant vjParentCenterV := 1*64;\n"
	"constant vjParentBottomV := 2*64;\n"
	"constant vjParentFullV := 3*64;\n"
	"constant vjParentVMask := 3*64;\n"
	"constant vjParentMask := vjParentHMask+vjParentVMask;\n"
	"constant vjSiblingNoH := 0*512;\n"
	"constant vjSiblingCenterH := 1*512;\n"
	"constant vjSiblingRightH := 2*512;\n"
	"constant vjSiblingFullH := 3*512;\n"
	"constant vjSiblingLeftH := 4*512;\n"
	"constant vjSiblingHMask := 7*512;\n"
	"constant vjSiblingNoV := 0*4096;\n"
	"constant vjSiblingCenterV := 1*4096;\n"
	"constant vjSiblingBottomV := 2*4096;\n"
	"constant vjSiblingFullV := 3*4096;\n"
	"constant vjSiblingTopV := 4*4096;\n"
	"constant vjSiblingVMask := 7*4096;\n"
	"constant vjSiblingMask := vjSiblingHMask+vjSiblingVMask;\n"
	"constant vjChildrenLasso := 1*32768;\n"
	"constant vjReflow := 1*65536;\n"
	"constant noLineLimits := 0*8388608;\n"
	"constant oneLineOnly := 1*8388608;\n"
	"constant oneWordOnly := 2*8388608;\n"
	"constant modeCopy := 0;\n"
	"constant modeOr := 1;\n"
	"constant modeXor := 2;\n"
	"constant modeBic := 3;\n"
	"constant modeNotCopy := 4;\n"
	"constant modeNotOr := 5;\n"
	"constant modeNotXor := 6;\n"
	"constant modeNotBic := 7;\n"
	"constant modeMask := 8;\n"
	"constant vfNone := 0;\n"
	"constant vfWhite := 1;\n"
	"constant vfLtGray := 2;\n"
	"constant vfGray := 3;\n"
	"constant vfDkGray := 4;\n"
	"constant vfBlack := 5;\n"
	"constant vfCustom := 14;\n"
	"constant vfMatte := 15;\n"
	"constant vfFillWhite := vfWhite;\n"
	"constant vfFillLtGray := vfLtGray;\n"
	"constant vfFillGray := vfGray;\n"
	"constant vfFillDkGray := vfDkGray;\n"
	"constant vfFillBlack := vfBlack;\n"
	"constant vfFillCustom := vfCustom;\n"
	"constant vfFillMask := 15;\n"
	"constant vfFillShift := 0;\n"
	"constant vfFrameWhite := vfWhite*16;\n"
	"constant vfFrameLtGray := vfLtGray*16;\n"
	"constant vfFrameGray := vfGray*16;\n"
	"constant vfFrameDkGray := vfDkGray*16;\n"
	"constant vfFrameBlack := vfBlack*16;\n"
	"constant vfFrameCustom := vfCustom*16;\n"
	"constant vfFrameMatte := vfMatte*16;\n"
	"constant vfFrameMask := 15*16;\n"
	"constant vfFrameShift := 4;\n"
	"func vfPen(x)((x)*256);\n"
	"constant vfPenMask := 15*256;\n"
	"constant vfPenShift := 8;\n"
	"constant vfLinesWhite := vfWhite*4096;\n"
	"constant vfLinesLtGray := vfLtGray*4096;\n"
	"constant vfLinesGray := vfGray*4096;\n"
	"constant vfLinesDkGray := vfDkGray*4096;\n"
	"constant vfLinesBlack := vfBlack*4096;\n"
	"constant vfLinesMask := 15*4096;\n"
	"constant vfLineShift := 12;\n"
	"func vfInset(x)((x)*65536);\n"
	"constant vfInsetMask := 3*65536;\n"
	"constant vfInsetShift := 16;\n"
	"func vfShadow(x)((x)*262144);\n"
	"constant vfShadowMask := 3*262144;\n"
	"constant vfShadowShift := 18;\n"
	"constant vfHiliteInvert := 0*1048576;\n"
	"constant vfHiliteBullet := 2*1048576;\n"
	"constant vfHiliteTriangle := 4*1048576;\n"
	"constant vfHiliteMask := 15*1048576;\n"
	"constant vfHiliteShift := 20;\n"
	"func vfRound(x)((x)*16777216);\n"
	"constant vfRoundMask := 15*16777216;\n"
	"constant vfRoundShift := 24;\n"
	"constant vNoFlags := 0;\n"
	"constant vVisible := 1 << 0;\n"
	"constant vReadOnly := 1 << 1;\n"
	"constant vApplication := 1 << 2;\n"
	"constant vCalculateBounds := 1 << 3;\n"
	"constant vNoKeys := 1 << 4;\n"
	"constant vClipping := 1 << 5;\n"
	"constant vFloating := 1 << 6;\n"
	"constant vWriteProtected := 1 << 7;\n"
	"constant vNothingAllowed := 0x00000000;\n"
	"constant vSingleUnit := 1 << 8;\n"
	"constant vClickable := 1 << 9;\n"
	"constant vStrokesAllowed := 1 << 10;\n"
	"constant vGesturesAllowed := 1 << 11;\n"
	"constant vCharsAllowed := 1 << 12;\n"
	"constant vNumbersAllowed := 1 << 13;\n"
	"constant vLettersAllowed := 1 << 14;\n"
	"constant vPunctuationAllowed := 1 << 15;\n"
	"constant vShapesAllowed := 1 << 16;\n"
	"constant vPhoneField := 1 << 18;\n"
	"constant vDateField := 1 << 19;\n"
	"constant vTimeField := 1 << 20;\n"
	"constant vAddressField := 1 << 21;\n"
	"constant vNameField := 1 << 22;\n"
	"constant vCapsRequired := 1 << 23;\n"
	"constant vCustomDictionaries := 1 << 24;\n"
	"constant vAnythingAllowed := 65535 << 9;\n"
	"constant vSelected := 1 << 25;\n"
	"constant vNoScripts := 1 << 27;\n"
	"constant vWidthIsParentWidth := 1 << 0;\n"
	"constant vNoSpaces := 1 << 1;\n"
	"constant fxColumnsShift := 0;\n"
	"func fxColumns(num)(num-1) << fxColumnsShift;\n"
	"constant fxColumnsMask := 0x1F << fxColumnsShift;\n"
	"constant fxRowsShift := 5;\n"
	"func fxRows(num)(num-1) << fxRowsShift;\n"
	"constant fxRowsMask := 0x1F << fxRowsShift;\n"
	"constant fxHStartPhaseShift := 10;\n"
	"constant fxHStartPhase := 1 << fxHStartPhaseShift;\n"
	"constant fxVStartPhaseShift := 11;\n"
	"constant fxVStartPhase := 1 << fxVStartPhaseShift;\n"
	"constant fxColAltHPhaseShift := 12;\n"
	"constant fxColAltHPhase := 1 << fxColAltHPhaseShift;\n"
	"constant fxColAltVPhaseShift := 13;\n"
	"constant fxColAltVPhase := 1 << fxColAltVPhaseShift;\n"
	"constant fxRowAltHPhaseShift := 14;\n"
	"constant fxRowAltHPhase := 1 << fxRowAltHPhaseShift;\n"
	"constant fxRowAltVPhaseShift := 15;\n"
	"constant fxRowAltVPhase := 1 << fxRowAltVPhaseShift;\n"
	"constant fxMoveHShift := 16;\n"
	"constant fxMoveH := 1 << fxMoveHShift;\n"
	"constant fxMoveVShift := 17;\n"
	"constant fxMoveV := 1 << fxMoveVShift;\n"
	"constant fxRevealLineShift := 18;\n"
	"constant fxRevealLine := 1 << fxRevealLineShift;\n"
	"constant fxWipeShift := 19;\n"
	"constant fxWipe := 1 << fxWipeShift;\n"
	"constant fxFromEdgeShift := 20;\n"
	"constant fxFromEdge := 1 << fxFromEdgeShift;\n"
	"constant fxStepsShift := 21;\n"
	"func fxSteps(num)((num-1) << fxStepsShift);\n"
	"constant fxStepsMask := 0x0F << fxStepsShift;\n"
	"constant fxStepTimeShift := 25;\n"
	"func fxStepTime(num)((num) << fxStepTimeShift);\n"
	"constant fxStepTimeMask := 0x0F << fxStepTimeShift;\n"
	"constant fxLeft := fxHStartPhase+fxMoveH;\n"
	"constant fxRight := fxMoveH;\n"
	"constant fxUp := fxVStartPhase+fxMoveV;\n"
	"constant fxDown := fxMoveV;\n"
	"constant fxCheckerboardEffect := fxColumns(8)+fxRows(8)+fxColAltVPhase+fxRowAltHPhase+fxDown;\n"
	"constant fxBarnDoorOpenEffect := fxColumns(2)+fxColAltHPhase+fxRowAltVPhase+fxLeft+fxWipe;\n"
	"constant fxBarnDoorCloseEffect := fxColumns(2)+fxColAltHPhase+fxRowAltVPhase+fxRight+fxWipe;\n"
	"constant fxVenetianBlindsEffect := fxRows(8)+fxDown;\n"
	"constant fxIrisOpenEffect := fxColumns(2)+fxRows(2)+fxUp+fxLeft+fxColAltHPhase+fxRowAltVPhase+fxRevealLine+fxWipe;\n"
	"constant fxIrisCloseEffect := fxColumns(2)+fxRows(2)+fxUp+fxLeft+fxRevealLine+fxWipe;\n"
	"constant fxPopDownEffect := fxDown+fxRevealLine;\n"
	"constant fxDrawerEffect := fxUp;\n"
	"constant fxZoomOpenEffect := fxColumns(2)+fxRows(2)+fxUp+fxLeft+fxColAltHPhase+fxRowAltVPhase;\n"
	"constant fxZoomCloseEffect := fxColumns(2)+fxRows(2)+fxUp+fxLeft;\n"
	"constant fxZoomVerticalEffect := fxColumns(1)+fxRows(2)+fxUp+fxRowAltVPhase;\n"
	"constant kNoSounds := 0;\n"
	"constant kFeedbackSounds := 1;\n"
	"constant kAllSounds := 2;\n"
	"constant cpNoCopyProtection := 0;\n"
	"constant cpNoCopies := 1;\n"
	"constant cpReadOnlyCopies := 2;\n"
	"constant cpOriginalOnlyCopies := 4;\n"
	"constant cpNewtonOnlyCopies := 8;\n"
	"constant keySpacer := 1 << 29;\n"
	"constant keyAutoHilite := 1 << 28;\n"
	"constant keyInsetUnit := 1 << 25;\n"
	"constant keyFramed := 1 << 23;\n"
	"constant keyRoundingUnit := 1 << 20;\n"
	"constant keyLeftOpen := 1 << 19;\n"
	"constant keyBottomOpen := 1 << 18;\n"
	"constant keyTopOpen := 1 << 17;\n"
	"constant keyRightOpen := 1 << 16;\n"
	"constant keyHUnit := 1 << 11;\n"
	"constant keyHHalf := 1 << 10;\n"
	"constant keyHQuarter := 1 << 9;\n"
	"constant keyHEighth := 1 << 8;\n"
	"constant keyVUnit := 1 << 3;\n"
	"constant keyVHalf := 1 << 2;\n"
	"constant keyVQuarter := 1 << 1;\n"
	"constant keyVEighth := 1 << 0;\n"
	"constant kNotKeyCode := -1;\n"
	"constant tsFamilyMask := 0x000003FF;\n"
	"constant tsFamilyShift := 0;\n"
	"constant tsSizeMask := 0x000FFC00;\n"
	"constant tsSizeShift := 10;\n"
	"constant tsFaceMask := 0x3FF00000;\n"
	"constant tsFaceShift := 20;\n"
	"constant tsSimple := 2 << tsFamilyShift;\n"
	"constant tsFancy := 1 << tsFamilyShift;\n"
	"func tsSize(num)((num) << tsSizeShift);\n"
	"constant tsPlain := 0x00000000;\n"
	"constant tsBold := 0x00100000;\n"
	"constant tsItalic := 0x00200000;\n"
	"constant tsUnderline := 0x00400000;\n"
	"constant tsOutline := 0x00800000;\n"
	"constant tsSuperScript := 0x08000000;\n"
	"constant tsSubScript := 0x10000000;\n"
	"constant tsUndefinedFace := 0x20000000;\n"
	"constant userFont9 := 0x00002401;\n"
	"constant userFont10 := 0x00002801;\n"
	"constant userFont12 := 0x00003001;\n"
	"constant userFont18 := 0x00004801;\n"
	"constant simpleFont9 := 0x00002402;\n"
	"constant simpleFont10 := 0x00002802;\n"
	"constant simpleFont12 := 0x00003002;\n"
	"constant simpleFont18 := 0x00004802;\n"
	"constant fancyFont9 := 0x00002401;\n"
	"constant fancyFont10 := 0x00002801;\n"
	"constant fancyFont12 := 0x00003001;\n"
	"constant fancyFont18 := 0x00004801;\n"
	"constant firstX := 0;\n"
	"constant firstY := 1;\n"
	"constant lastX := 2;\n"
	"constant lastY := 3;\n"
	"constant finalX := 4;\n"
	"constant finalY := 5;\n"
	"constant kNotifyLog := 1;\n"
	"constant kNotifyMessage := 2;\n"
	"constant kNotifyAlert := 3;\n"
	"constant kNotifyQAlert := 4;\n"
	"constant kModalAlert := 5;\n"
	"constant kModalQAlert := 6;\n"
	"constant kElementTypeWidth := 3;\n"
	"constant kElementFormatWidth := 3;\n"
	"constant kElementShift := kElementTypeWidth + kElementFormatWidth;\n"
	"constant kElementTypeMask :=  (1 << kElementTypeWidth) - 1 ;\n"
	"constant kElementFormatMask :=  (1 << kElementFormatWidth) - 1 ;\n"
	"constant kElementNothing := 0;\n"
	"constant kElementDay := 1;\n"
	"constant kElementDayOfWeek := 2;\n"
	"constant kElementMonth := 3;\n"
	"constant kElementYear := 4;\n"
	"constant kElementEra := 5;\n"
	"constant kElementHour := 1;\n"
	"constant kElementMinute := 2;\n"
	"constant kElementSecond := 3;\n"
	"constant kElementAMPM := 4;\n"
	"constant kElementSuffix := 5;\n"
	"constant kFormatDefault := 0;\n"
	"constant kFormatLong := 1;\n"
	"constant kFormatAbbr := 2;\n"
	"constant kFormatTerse := 3;\n"
	"constant kFormatShort := 4;\n"
	"constant kFormatNumeric := 5;\n"
	"constant kFormatLongDate := 1;\n"
	"constant kFormatAbbrDate := 2;\n"
	"constant kFormatNumericDate := 3;\n"
	"constant kFormatNumericYear := 4;\n"
	"constant kFormatLongMonth := 5;\n"
	"constant kFormatAbbrMonth := 6;\n"
	"constant kFormatNumericDay := 7;\n"
	"constant kFormatLongDayOfWeek := 8;\n"
	"constant kFormatAbbrDayOfWeek := 9;\n"
	"constant kFormatLongTime := 21;\n"
	"constant kFormatShortTime := 22;\n"
	"constant kFormatHour := 23;\n"
	"constant kFormatMinute := 24;\n"
	"constant kFormatSecond := 25;\n"
	"constant kIncludeAllElements := 0;\n"
	"constant kMaxElementStrSize := 31;\n"
	"constant kMaxDateTimeStrSize := 63;\n"
	"constant kLeadZero := 0;\n"
	"constant kNoLeadZero := 1;\n"
	"constant kCycle24 := 0;\n"
	"constant kCycle12 := 1;\n"
	"constant kUseHourZero := 0;\n"
	"constant kUseHour12 := 1;\n"
	"constant kUseHour24 := 2;\n"
	"constant kShowCentury := 0;\n"
	"constant kNoCentury := 1;\n"
	"constant sRoman := 0;\n"
	"constant sJapanese := 1;\n"
	"constant sChinese := 2;\n"
	"constant sKorean := 3;\n"
	"constant sArabic := 4;\n"
	"constant sHebrew := 5;\n"
	"constant kFirstClassID := 64;\n"
	"constant clView := 10 + kFirstClassID;\n"
	"constant clPictureView := 12 + kFirstClassID;\n"
	"constant clEditView := 13 + kFirstClassID;\n"
	"constant clKeyboardView := 15 + kFirstClassID;\n"
	"constant clMonthView := 16 + kFirstClassID;\n"
	"constant clParagraphView := 17 + kFirstClassID;\n"
	"constant clPolygonView := 18 + kFirstClassID;\n"
	"constant clRemoteView := 24 + kFirstClassID;\n"
	"constant clPickView := 27 + kFirstClassID;\n"
	"constant clGaugeView := 28 + kFirstClassID;\n"
	"constant clOutline := 41 + kFirstClassID;\n"
	"constant kModemECProtocolNone := 0x00000001;\n"
	"constant kModemECProtocolMNP := 0x00000002;\n"
	"constant kModemECProtocolLAPM := 0x00000004;\n"
	"constant kModemECProtocolExternal := 0x00000008;\n"
	"constant kSpeakerVolumeLow := \"1\";\n"
	"constant kSpeakerVolumeMedium := \"2\";\n"
	"constant kSpeakerVolumeHigh := \"3\";\n"
	"constant kModemCntSpeed300 := 0x00000001;\n"
	"constant kModemCntSpeed1200 := 0x00000002;\n"
	"constant kModemCntSpeed2400 := 0x00000004;\n"
	"constant kModemCntSpeed4800 := 0x00000008;\n"
	"constant kModemCntSpeed7200 := 0x00000010;\n"
	"constant kModemCntSpeed9600 := 0x00000020;\n"
	"constant kModemCntSpeed12000 := 0x00000040;\n"
	"constant kModemCntSpeed14400 := 0x00000080;\n"
	"constant kModemCntSpeed16800 := 0x00000100;\n"
	"constant kModemCntSpeed19200 := 0x00000200;\n"
	"constant kModemCntSpeed21600 := 0x00000400;\n"
	"constant kModemCntSpeed24000 := 0x00000800;\n"
	"constant kModemCntSpeed26800 := 0x00001000;\n"
	"constant kModemCntSpeed29000 := 0x00002000;\n"
	"constant kModemCntSpeed31400 := 0x00004000;\n"
	"constant kCMOMNPAllocate := \"mnpa\";\n"
	"constant kCMOMNPCompression := \"mnpc\";\n"
	"constant kCMOMNPDataRate := \"eter\";\n"
	"constant kCMOMNPStatistics := \"mnps\";\n"
	"constant kMNPCompressionNone := 0x00000001;\n"
	"constant kMNPCompressionMNP5 := 0x00000002;\n"
	"constant kMNPCompressionV42bis := 0x00000008;\n"
	"constant kCMS_CapabilityType_ServiceId := \"serv\";\n"
	"constant kCMS_CapabilityType_Version := \"vern\";\n"
	"constant kCMS_UnknownVersion := \"zzzz\";\n"
	"constant kCMSAppleTalkID := \"atlk\";\n"
	"constant kCMSFaxID := \"faxs\";\n"
	"constant kCMSModemID := \"mods\";\n"
	"constant kCMSSlowIR := \"slir\";\n"
	"constant kCMSMNPID := \"mnps\";\n"
	"constant kCMSBasicSerial := \"ser \";\n"
	"constant kCMSAsyncSerial := \"aser\";\n"
	"constant kCMSFramedAsyncSerial := \"fser\";\n"
	"constant kCMSLocalTalkID := \"ltlk\";\n"
	"constant kCMSKeyboard := \"kybd\";\n"
	"constant kCMSBlackbird := \"blkb\";\n"
	"constant kCMSVAppleTalk := \"1\";\n"
	"constant kCMSVFaxID := \"1\";\n"
	"constant kCMSVModemID := \"1\";\n"
	"constant kCMSVSlowIR := \"1\";\n"
	"constant kCMSVMNPID := \"1\";\n"
	"constant kCMSVBasicSerial := \"1\";\n"
	"constant kCMSVAsyncSerial := \"1\";\n"
	"constant kCMSVFramedAsyncSerial := \"1\";\n"
	"constant kCMSVLocalTalkID := \"1\";\n"
	"constant kCMSVKeyboard := \"1\";\n"
	"constant kCMSVBlackbird := \"1\";\n"
	"constant kSubArrayOption := \"suba\";\n"
	"constant kOptionType := 0x0C000000;\n"
	"constant kConfigType := 0x10000000;\n"
	"constant kTypeMask := 0x3C000000;\n"
	"constant kOpCodeMask := 0x0000ff00;\n"
	"constant kOpCodeResultMask := 0x000000ff;\n"
	"constant opInvalid := 0x0000;\n"
	"constant opSetNegotiate := 0x0100;\n"
	"constant opSetRequired := 0x0200;\n"
	"constant opGetDefault := 0x0300;\n"
	"constant opGetCurrent := 0x0400;\n"
	"constant opProcess := 0x0500;\n"
	"constant unicodeNUL := $\\00;\n"
	"constant unicodeSOH := $\\01;\n"
	"constant unicodeSTX := $\\02;\n"
	"constant unicodeETX := $\\03;\n"
	"constant unicodeEOT := $\\04;\n"
	"constant unicodeENQ := $\\05;\n"
	"constant unicodeACK := $\\06;\n"
	"constant unicodeBEL := $\\07;\n"
	"constant unicodeBS := $\\08;\n"
	"constant unicodeHT := $\\09;\n"
	"constant unicodeLF := $\\0A;\n"
	"constant unicodeVT := $\\0B;\n"
	"constant unicodeFF := $\\0C;\n"
	"constant unicodeCR := $\\0D;\n"
	"constant unicodeSO := $\\0E;\n"
	"constant unicodeSI := $\\0F;\n"
	"constant unicodeDLE := $\\10;\n"
	"constant unicodeDC1 := $\\11;\n"
	"constant unicodeDC2 := $\\12;\n"
	"constant unicodeDC3 := $\\13;\n"
	"constant unicodeDC4 := $\\14;\n"
	"constant unicodeNAK := $\\15;\n"
	"constant unicodeSYN := $\\16;\n"
	"constant unicodeETB := $\\17;\n"
	"constant unicodeCAN := $\\18;\n"
	"constant unicodeEM := $\\19;\n"
	"constant unicodeSUB := $\\1A;\n"
	"constant unicodeESC := $\\1B;\n"
	"constant unicodeFS := $\\1C;\n"
	"constant unicodeGS := $\\1D;\n"
	"constant unicodeRS := $\\1E;\n"
	"constant unicodeUS := $\\1F;\n"
	"constant kDefaultXOnChar := 0x11;\n"
	"constant kDefaultXOffChar := 0x13;\n"
	"constant kDefaultFramingChar := 0x10;\n"
	"constant kDefaultEOMChar := 0x03;\n"
	"constant kDefaultSOMChar := 0x16;\n"
	"constant kDefaultSOHChar := 0x02;\n"
	"constant kNoChannel := 0;\n"
	"constant kSCCSideA := 1;\n"
	"constant kSCCSideB := 2;\n"
	"constant kSCCServiceNotSpecified := 0;\n"
	"constant kSCCServiceStandard := 1;\n"
	"constant kSCCServicePtToPtIR := 2;\n"
	"constant kSCCServiceModem := 3;\n"
	"constant kSCCServicePrinter := 4;\n"
	"constant k1StopBits := 0;\n"
	"constant k1pt5StopBits := 1;\n"
	"constant k2StopBits := 2;\n"
	"constant kNoParity := 0;\n"
	"constant kOddParity := 1;\n"
	"constant kEvenParity := 2;\n"
	"constant k5DataBits := 5;\n"
	"constant k6DataBits := 6;\n"
	"constant k7DataBits := 7;\n"
	"constant k8DataBits := 8;\n"
	"constant kExternalClock := 1;\n"
	"constant k300bps := 300;\n"
	"constant k600bps := 600;\n"
	"constant k1200bps := 1200;\n"
	"constant k2400bps := 2400;\n"
	"constant k4800bps := 4800;\n"
	"constant k7200bps := 7200;\n"
	"constant k9600bps := 9600;\n"
	"constant k12000bps := 12000;\n"
	"constant k14400bps := 14400;\n"
	"constant k19200bps := 19200;\n"
	"constant k38400bps := 38400;\n"
	"constant k57600bps := 57600;\n"
	"constant k115200bps := 115200;\n"
	"constant k230400bps := 230400;\n"
	"constant kSerialEventBreakStartedMask := 0x00000001;\n"
	"constant kSerialEventBreakEndedMask := 0x00000002;\n"
	"constant kSerialEventDCDNegatedMask := 0x00000004;\n"
	"constant kSerialEventDCDAssertedMask := 0x00000008;\n"
	"constant kSerialEventHSKiNegatedMask := 0x00000010;\n"
	"constant kSerialEventHSKiAssertedMask := 0x00000020;\n"
	"constant kSerialEventExtClkDetectEnableMask := 0x00000040;\n"
	"constant kSerialEventGPiWakeupEnableMask := 0x00000080;\n"
	"constant irUsingNegotiateIR := 0x00;\n"
	"constant irUsingSharpIR := 0x01;\n"
	"constant irUsingNewtIR := 0x02;\n"
	"constant irJ1ProtocolType := bor(irUsingSharpIR,irUsingNewtIR);\n"
	"constant irUsing9600 := 0x01;\n"
	"constant irUsing19200 := 0x02;\n"
	"constant irUsing38400 := 0x04;\n"
	"constant irJ1ProtocolOptions := bor(irUsing9600,irUsing19200);\n"
	"constant kCMOSerialHardware := \"scc \";\n"
	"constant kCMOSerialBuffers := \"sbuf\";\n"
	"constant kCMOSerialIOParms := \"siop\";\n"
	"constant kCMOSerialBitRate := \"sbps\";\n"
	"constant kCMOSerialBreak := \"sbrk\";\n"
	"constant kCMOSerialBytesAvailable := \"sbav\";\n"
	"constant kCMOSerialDiscard := \"sdsc\";\n"
	"constant kCMOBreakFraming := \"sbkf\";\n"
	"constant kCMOSerialDTRControl := \"sdtr\";\n"
	"constant kCMOSerialMiscConfig := \"smsc\";\n"
	"constant kCMOSerialEventEnables := \"sevt\";\n"
	"constant kCMOSerialIOStats := \"sios\";\n"
	"constant kCMOOutputFlowControlParms := \"oflc\";\n"
	"constant kCMOInputFlowControlParms := \"iflc\";\n"
	"constant kCMOFramingParms := \"fram\";\n"
	"constant kCMOFramedAsyncStats := \"frst\";\n"
	"constant kCMOLocalTalkStats := \"ltst\";\n"
	"constant kCMOLocalTalkNodeID := \"ltid\";\n"
	"constant kCMOLocalTalkMiscConfig := \"ltms\";\n"
	"constant kCMOSlowIRProtocolType := \"irpt\";\n"
	"constant kCMOSlowIRStats := \"irst\";\n"
	"constant kCMOSlowIRSniff := \"irsn\";\n"
	"constant kCMOSlowIRBitBang := \"irtv\";\n"
	"constant kCMOEndpointName := \"endp\";\n"
	"constant kCMOAppleTalkADSP := [97,100,115,112];\n"
	"constant kADSPEndpoint := \"TADSPEndpoint\";\n"
	"constant kMNPDoAllocate := [1, 0, 0, 0];\n"
	"constant kMNPDontAllocate := [0, 0, 0, 0];\n"
	"constant k1KOutputBuffer := [ 0, 0, 2, 0, 0, 0, 4, 0, 0, 0, 0, 8 ];\n"
	"constant k2KOutputbuffer := [ 0, 0, 2, 0, 0, 0, 8, 0, 0, 0, 0, 8 ];\n"
	"constant T_NOFLAG := 0x00000000;\n"
	"constant T_CANSENDZERO := 0x00000001;\n"
	"constant T_CANEOM := 0x00000002;\n"
	"constant T_EXPCHANNEL := 0x00000004;\n"
	"constant T_RELIABLE := 0x00000008;\n"
	"constant T_CANBIND := 0x00000010;\n"
	"constant T_CANLISTEN := 0x00000011;\n"
	"constant T_CANCONNECT := 0x00000012;\n"
	"constant T_CANRELEASE := 0x00000014;\n"
	"constant T_FRAMING := 0x00000018;\n"
	"constant T_FLOWCONTROL := 0x00000020;\n"
	"constant T_MORE := 0x00000001;\n"
	"constant T_FRAME := 0x00000002;\n"
	"constant T_EXP := 0x00000004;\n"
	"constant T_COTS := 1;\n"
	"constant T_COTS_ORD := 2;\n"
	"constant T_CLTS := 3;\n"
	"constant T_TRANS := 5;\n"
	"constant T_TRANS_ORD := 6;\n"
	"constant T_UNINIT := 0;\n"
	"constant T_UNBND := 1;\n"
	"constant T_IDLE := 2;\n"
	"constant T_OUTCON := 3;\n"
	"constant T_INCON := 4;\n"
	"constant T_DATAXFER := 5;\n"
	"constant T_OUTREL := 6;\n"
	"constant T_INREL := 7;\n"
	"constant T_INFLUX := 8;\n"
	"constant T_YES := 1;\n"
	"constant T_NO := 0;\n"
	"constant T_UNUSED := -1;\n"
	"constant T_NULL := 0;\n"
	"constant T_ABSREQ := 0x00008000;\n"
	"constant T_INFINITE := -1;\n"
	"constant T_INVALID := -2;\n"
	"constant kCMOModemECType := \"mecp\";\n"
	"constant kCMOModemDialing := \"mdo \";\n"
	"constant kCMOModemConnectType := \"mcto\";\n"
	"constant kCMOModemPrefs := \"mpre\";\n"
	"constant kCMOModemProfile := \"mpro\";\n"
	"constant kCMOModemVoiceSupport := \"mvso\";\n"
	"constant kCMATitleLabel := \"titl\";\n"
	"constant kCMATypeLabel := \"typa\";\n"
	"constant kCMADiscLabel := \"disc\";\n"
	"constant kCMAKindLabel := \"kind\";\n"
	"constant kCMARouteLabel := \"rout\";\n"
	"constant kCMAltLabel := \"alt \";\n"
	"constant kCMABBServiceName := \"BBSV\";\n"
	"constant kNamedAppleTalkAddress := 1;\n"
	"constant kRawAppleTalkAddress := 2;\n"
	"constant kPhoneNumber := 3;\n"
	"constant kNBPEntityName := 1;\n"
	"constant kNameTypeZone := 2;\n"
	"constant kSunday := 0x00000800;\n"
	"constant kMonday := 0x00000400;\n"
	"constant kTuesday := 0x00000200;\n"
	"constant kWednesday := 0x00000100;\n"
	"constant kThursday := 0x00000080;\n"
	"constant kFriday := 0x00000040;\n"
	"constant kSaturday := 0x00000020;\n"
	"constant kEveryday := 0x00000FE0;\n"
	"constant kFirstWeek := 0x00000010;\n"
	"constant kSecondWeek := 0x00000008;\n"
	"constant kThirdWeek := 0x00000004;\n"
	"constant kFourthWeek := 0x00000002;\n"
	"constant kLastWeek := 0x00000001;\n"
	"constant kEveryWeek := 0x0000001F;\n"
	"constant kGestalt_Base := 0x01000001;\n"
	"constant kGestalt_Version := kGestalt_Base + 1;\n"
	"constant kGestalt_SystemInfo := kGestalt_Base + 2;\n"
	"constant kGestalt_RebootInfo := kGestalt_Base + 3;\n"
	"constant kGestalt_NewtonScriptVersion := kGestalt_Base + 4;\n"
	"constant kGestalt_Manufacturer_Apple := 0x10000000;\n"
	"constant kGestalt_Manufacturer_Sharp := 0x10000100;\n"
	"constant kGestalt_MachineType_MessagePad := 0x10001000;\n"
	"constant aeNoCommand := 0;\n"
	"constant recognitionAEvents := 10;\n"
	"constant aeClick := 1 + recognitionAEvents;\n"
	"constant aeStroke := 2 + recognitionAEvents;\n"
	"constant aeScrub := 3 + recognitionAEvents;\n"
	"constant aeGesture := aeScrub;\n"
	"constant aeCaret := 5 + recognitionAEvents;\n"
	"constant aeLine := 6 + recognitionAEvents;\n"
	"constant aeShape := 7 + recognitionAEvents;\n"
	"constant aeWord := 8 + recognitionAEvents;\n"
	"constant aeMath := 9 + recognitionAEvents;\n"
	"constant aeGetContext := 10 + recognitionAEvents;\n"
	"constant aeInk := 11 + recognitionAEvents;\n"
	"constant aeString := 12 + recognitionAEvents;\n"
	"constant keyAEvents := 30;\n"
	"constant aeKeyUp := 1 + keyAEvents;\n"
	"constant aeKeyDown := 2 + keyAEvents;\n"
	"constant aeKeyboardEnable := 3 + keyAEvents;\n"
	"constant viewAEvents := 40;\n"
	"constant aeAddChild := 1 + viewAEvents;\n"
	"constant aeDropChild := 2 + viewAEvents;\n"
	"constant aeHide := 3 + viewAEvents;\n"
	"constant aeShow := 4 + viewAEvents;\n"
	"constant aeScrollUp := 5 + viewAEvents;\n"
	"constant aeScrollDown := 6 + viewAEvents;\n"
	"constant aeHilite := 7 + viewAEvents;\n"
	"constant aeRemoveAllHilites := 8 + viewAEvents;\n"
	"constant aeTap := 9 + viewAEvents;\n"
	"constant aeDoubleTap := 10 + viewAEvents;\n"
	"constant aeOverview := 11 + viewAEvents;\n"
	"constant aeStartHilite := 12 + viewAEvents;\n"
	"constant aeClickUp := 13 + viewAEvents;\n"
	"constant aePickItem := 14 + viewAEvents;\n"
	"constant aeTapDrag := 15 + viewAEvents;\n"
	"constant aePrintAllPagesNow := 16 + viewAEvents;\n"
	"constant editViewAEvents := 60;\n"
	"constant aeAddData := 1 + editViewAEvents;\n"
	"constant aeDuplicateData := 2 + editViewAEvents;\n"
	"constant aeRemoveData := 3 + editViewAEvents;\n"
	"constant aeMoveData := 4 + editViewAEvents;\n"
	"constant aeDropData := 5 + editViewAEvents;\n"
	"constant aeScaleData := 6 + editViewAEvents;\n"
	"constant aeSetVertex := 7 + editViewAEvents;\n"
	"constant aeRemoveVertices := 8 + editViewAEvents;\n"
	"constant aeReplaceText := 10 + editViewAEvents;\n"
	"constant aeAddHilite := 11 + editViewAEvents;\n"
	"constant aeRemoveHilite := 12 + editViewAEvents;\n"
	"constant aeChangeStyle := 13 + editViewAEvents;\n"
	"constant aeDeferredRecognition := 14 + editViewAEvents;\n"
	"constant aeChangePen := 15 + editViewAEvents;\n"
	"constant aeMoveChild := 16 + editViewAEvents;\n"
	"constant debugAEvents := 80;\n"
	"constant aeRedrawScreen := 1 + debugAEvents;\n"
	"constant aeFlipBuffering := 2 + debugAEvents;\n"
	"constant aeLCDScreen := 3 + debugAEvents;\n"
	"constant aeToggleTracing := 4 + debugAEvents;\n"
	"constant aeShowDebugWindow := 5 + debugAEvents;\n"
	"constant aeDumpFields := 6 + debugAEvents;\n"
	"constant aeDumpData := 7 + debugAEvents;\n"
	"constant aeDumpWindows := 8 + debugAEvents;\n"
	"constant aeSetNewtMachine := 9 + debugAEvents;\n"
	"constant aeSetNewtonMachine := 10 + debugAEvents;\n"
	"constant aeSetMacMachine := 11 + debugAEvents;\n"
	"constant aeSetDOSMachine := 12 + debugAEvents;\n"
	"constant aeSetRolexMachine := 13 + debugAEvents;\n"
	"constant macAEvents := 100;\n"
	"constant aeDoNothing := 1 + macAEvents;\n"
	"constant aeAbout := 2 + macAEvents;\n"
	"constant aeOpenDeskAccessory := 3 + macAEvents;\n"
	"constant aeNew := 4 + macAEvents;\n"
	"constant aeOpen := 5 + macAEvents;\n"
	"constant aeClose := 6 + macAEvents;\n"
	"constant aeSave := 7 + macAEvents;\n"
	"constant aeSaveAs := 8 + macAEvents;\n"
	"constant aeRevert := 9 + macAEvents;\n"
	"constant aePageSetUp := 10 + macAEvents;\n"
	"constant aePrint := 11 + macAEvents;\n"
	"constant aeQuit := 12 + macAEvents;\n"
	"constant aeRunScript := 13 + macAEvents;\n"
	"constant aeUndo := 14 + macAEvents;\n"
	"constant aeCut := 15 + macAEvents;\n"
	"constant aeCopy := 16 + macAEvents;\n"
	"constant aePaste := 17 + macAEvents;\n"
	"constant aeClear := 18 + macAEvents;\n"
	"constant aeSelectAll := 19 + macAEvents;\n"
	"constant aeShowClipboard := 20 + macAEvents;\n"
	"constant kPopChar := $\\uFC01;\n"
	"constant kPopSpace := $\\uFC02;\n"
	"constant kCheckMarkChar := $\\uFC0B;\n"
	"constant kInfoDotChar := $\\uFC05;\n"
	"func GetDateStringSpec(elemArray)\n"
	"begin\n"
	"local spec := 0;\n"
	"local val;\n"
	"foreach val in elemArray do\n"
	"spec := spec<<kElementShift + val[1]<<kElementTypeWidth + val[0];\n"
	"spec;\n"
	"end;\n"
	"func SetBounds(l,t,r,b){left: l, top: t, right: r, bottom: b};\n"
	"func RelBounds(l,t,wid,hgt){left: l, top: t, right: l + wid, bottom: t + hgt};\n"
	"func PictBounds(rsrcName,l,t)\n"
	"begin\n"
	"    local pict := GetNamedResource(\"PICT\",rsrcName,'rawPICT);\n"
	"    if pict then\n"
	"        SetBounds(l,t,l+ ExtractWord(pict,2+6)-ExtractWord(pict,2+2),t + ExtractWord(pict,2+4)-ExtractWord(pict,2+0) )\n"
	"end;\n"
	"constant kNone := 0;\n"
	"constant kMuLaw := 1;\n"
	"constant kLPC := 2;\n"
	"constant kADPCM := 3;\n"
	"constant kSoundCompression6To1 := 4;\n"
	"constant kFixed := 0;\n"
	"constant k8Bit := 1;\n"
	"constant k16Bit := 2;\n"
	"constant k24Bit := 3;\n"
	"constant kCodecRate := 0x1F400000;\n"
	"constant kFloatCodecRate := 8000.0;\n"
	"constant kFloat11kRate := 11013.21586;\n"
	"constant kFloat22kRate := 22026.43172;\n"
	"func GetSoundDeprecated(name)\n"
	"begin\n"
	"    local soundSample := GetSndAsSamples(name);\n"
	"    if soundSample then\n"
	"    {\n"
	"        sndFrameType:    'simpleSound,\n"
	"        samples:         soundSample,\n"
	"        samplingRate:    kFloat22kRate,\n"
	"        dataType:        k8Bit,\n"
	"        compressionType: kNone\n"
	"    };\n"
	"end;\n"
	"func GetSound11Deprecated(name)\n"
	"begin\n"
	"    local soundSample := GetSndAsSamplesRate11KHz(name);\n"
	"    if soundSample then\n"
	"    {\n"
	"        sndFrameType:    'simpleSound,\n"
	"        samples:         soundSample,\n"
	"        samplingRate:    kFloat11kRate,\n"
	"        dataType:        k8Bit,\n"
	"        compressionType: kNone\n"
	"    };\n"
	"end;\n"
	"constant vjNoLineLimits := (0*8388608);\n"
	"constant unitFirstX := 0;\n"
	"constant unitFirstY := 1;\n"
	"constant unitFinalX := 4;\n"
	"constant unitFinalY := 5;\n"
	"constant kNoflag := 0x00000000;\n"
	"constant kCanSendZero := 0x00000001;\n"
	"constant kCanEOM := 0x00000002;\n"
	"constant kExpChannel := 0x00000004;\n"
	"constant kReliable := 0x00000008;\n"
	"constant kCanBind := 0x00000010;\n"
	"constant kCanListen := 0x00000011;\n"
	"constant kCanConnect := 0x00000012;\n"
	"constant kCanRelease := 0x00000014;\n"
	"constant kFraming := 0x00000018;\n"
	"constant kFlowControl := 0x00000020;\n"
	"constant kMore := 0x00000001;\n"
	"constant kFrame := 0x00000002;\n"
	"constant kExp := 0x00000004;\n"
	"constant kCots := 1;\n"
	"constant kCotsOrd := 2;\n"
	"constant kClts := 3;\n"
	"constant kTrans := 5;\n"
	"constant kTransOrd := 6;\n"
	"constant kUninit := 0;\n"
	"constant kUnbnd := 1;\n"
	"constant kIdle := 2;\n"
	"constant kOutCon := 3;\n"
	"constant kInCon := 4;\n"
	"constant kDataXfer := 5;\n"
	"constant kOutRel := 6;\n"
	"constant kInRel := 7;\n"
	"constant kInFlux := 8;\n"
	"constant kYes := 1;\n"
	"constant kNo := 0;\n"
	"constant kUnused := -1;\n"
	"constant kNull := 0;\n"
	"constant kAbsReq := 0x00008000;\n"
	"constant kInfinite := -1;\n"
	"constant kInvalid := -2;\n"
	"constant kfirstButtonLeft := 25;\n"
	"constant kstdButtonTop := 2;\n"
	"constant kstdButtonBottom := 15;\n"
	"constant kstdButtonHeight := 13;\n"
	"constant kStdButtonExtra := 6;\n"
	"constant kstdButtonSpacing := 6;\n"
	"func ButtonBounds(width)\n"
	"begin\n"
	"    if (width > 0) then\n"
	"        SetBounds(6, kstdButtonTop, 6 + width, kstdButtonBottom);\n"
	"    else\n"
	"        SetBounds(kfirstButtonLeft, kstdButtonTop, kfirstButtonLeft - width, kstdButtonBottom);\n"
	"end;\n"
	"constant kFaceNormal := 0x000;\n"
	"constant kFaceBold := 0x001;\n"
	"constant kFaceItalic := 0x002;\n"
	"constant kFaceUnderline := 0x004;\n"
	"constant kFaceOutline := 0x008;\n"
	"constant kFaceSuperScript := 0x080;\n"
	"constant kFaceSubScript := 0x100;\n"
	//"func DefConst(sym,expr);\n"
	"constant kMacRomanEncoding := 1;\n"
	"constant kASCIIEncoding := 2;\n"
	"constant kPCRomanEncoding := 3;\n"
	"constant kWizardEncoding := 4;\n"
	"constant kShiftJISEncoding := 5;\n"
	"constant kUserDictionary := 31;\n"
	"constant kCommonDictionary := 0;\n"
	"constant kCountriesDictionary := 8;\n"
	"constant kDaysMonthsDictionary := 34;\n"
	"constant kFirstNamesDictionary := 48;\n"
	"constant kLastNamesDictionary := 7;\n"
	"constant kSharedPropersDictionary := 1;\n"
	"constant kLocalPropersDictionary := 2;\n"
	"constant kLocalCitiesDictionary := 41;\n"
	"constant kLocalCompaniesDictionary := 42;\n"
	"constant kLocalStatesDictionary := 43;\n"
	"constant kLocalStatesAbbrevsDictionary := 44;\n"
	"constant kLocalDateDictionary := 110;\n"
	"constant kLocalTimeDictionary := 111;\n"
	"constant kLocalMoneyDictionary := 112;\n"
	"constant kLocalNumberDictionary := 113;\n"
	"constant FE_INEXACT := 0x00000010;\n"
	"constant FE_DIVBYZERO := 0x00000002;\n"
	"constant FE_UNDERFLOW := 0x00000008;\n"
	"constant FE_OVERFLOW := 0x00000004;\n"
	"constant FE_INVALID := 0x00000001;\n"
	"constant kInputFrame := 'InputFrame;\n"
	"constant kOutputFrame := 'OutputFrame;\n"
	"constant kFrameExchange_Initiate := 'InAndOut_MeFirst;\n"
	"constant kFrameExchange_Confirm := 'InAndOut_MeSecond;\n"
	"constant kPropagateUp := true;\n"
	"constant kNoPropagateUp := nil;\n"
	"constant kPropagateDown := true;\n"
	"constant kNoPropagateDown := nil;\n"
	"constant lvNoMarkers := 1;\n"
	"constant lvShowChecks := 2;\n"
	"constant lvShowTopPriority := 4;\n"
	"constant lvShowSubPriority := 8;\n"
	"constant lvShowPriority := 12;\n"
	"constant lvAdjustTopicBounds := 16;\n"
	"constant kUseLocalTalk := 1;\n"
	"constant kUseMNPSerial := 3;\n"
	"constant kUseIR := 4;\n"
	"constant kUseModem := 5;\n"
	"constant kNoConnection := 0;\n"
	"constant kSettingUp := 1;\n"
	"constant kSyncing := 2;\n"
	"constant kRestoring := 3;\n"
	"constant kLoadingPackage := 4;\n"
	"constant kLoadingPatch := 6;\n"
	"constant kFileBrowsing := 8;\n"
	"constant kOKForDevelopers := 20;\n"
	"constant kMacPlatform := 0;\n"
	"constant kWindowsPlatform := 1;\n"
	"constant kPackageFiles := 0;\n"
	"constant kImportFiles := 1;\n"
	"constant kRestoreFiles := 2;\n"
	"constant kOther := 3;\n"
	"constant kDesktop := 0;\n"
	"constant kFile := 1;\n"
	"constant kFolder := 2;\n"
	"constant kDisk := 3;\n"
	"constant kSystemInvertStyle := \"{transferMode: modeXor,penPattern: vfNone,fillPattern: vfBlack}\";\n"
	"constant vjNoRatio := 0;\n"
	"constant vjLeftRatio := 67108864;\n"
	"constant vjRightRatio := 134217728;\n"
	"constant vjTopRatio := 268435456;\n"
	"constant vjBottomRatio := 2 << 28;\n"
	"constant vjParentClip := 256;\n"
	"constant kStandardCharSetInfo := 0;\n"
	"constant kPalmerCharSetInfo := 1;\n"
	"constant kUCBlockCharSetInfo := 2;\n"
	"constant kUpperCaseCharSetInfo := 3;\n"
	"constant kLetterSpaceCursiveOptionDefault := 4;\n"
	"constant kLetterSetSelectionDefault := 0;\n"
	"constant kLong := 1;\n"
	"constant kULong := 2;\n"
	"constant kShort := 3;\n"
	"constant kByte := 4;\n"
	"constant kBoolean := 5;\n"
	"constant kHighInt := 6;\n"
	"constant kHexLong := 7;\n"
	"constant kSplitLong := 8;\n"
	"constant kSplitByteLong := 9;\n"
	"constant kStruct := 10;\n"
	"constant kArray := 11;\n"
	"constant kRef := 12;\n"
	"constant kChar := 13;\n"
	"constant kCString := 14;\n"
	"constant kUnicodeChar := 15;\n"
	"constant kUnicodeString := 16;\n"
	"constant kCommScriptNoActiveInputSpec := -54000;\n"
	"constant kCommScriptBadForm := -54001;\n"
	"constant kCommScriptZeroLengthData := -54002;\n"
	"constant kCommScriptExpectedSpec := -54003;\n"
	"constant kCommScriptInvalidOption := -54004;\n"
	"constant kCommScriptInvalidEndSequence := -54005;\n"
	"constant kCommScriptInappropriatePartial := -54006;\n"
	"constant kCommScriptInappropriateTermination := -54007;\n"
	"constant kCommScriptInappropriateTarget := -54008;\n"
	"constant kCommScriptInappropriateFilter := -54009;\n"
	"constant kCommScriptExpectedTarget := -54010;\n"
	"constant kCommScriptExpectedTemplate := -54011;\n"
	"constant kCommScriptInputSpecAlreadyActive := -54012;\n"
	"constant kCommScriptInvalidProxy := -54013;\n"
	"constant kCommScriptNoEndpointAvailable := -54014;\n"
	"constant kCommScriptInappropriateCall := -54015;\n"
	"constant kOutLstn := 9;\n"
	"constant kFrameExchange := 'autoInAndOut;\n"
	"constant kDataExchange := 'InAndOut;\n"
	"constant kCanadaCountryId := 10;\n"
	"constant kJapanCountryId := 81;\n"
	"constant kUSACountryId := 1;\n"
	"constant kModemFaxClass0 := 0x00000001;\n"
	"constant kModemFaxClass1 := 0x00000002;\n"
	"constant kModemFaxClass2 := 0x00000004;\n"
	"constant kModemECInternalOnly := 0x00000010;\n"
	"constant kV21Ch2Mod := 0x00000001;\n"
	"constant kV27Ter24Mod := 0x00000002;\n"
	"constant kV27Ter48Mod := 0x00000004;\n"
	"constant kV29_72Mod := 0x00000008;\n"
	"constant kV17_72Mod := 0x00000010;\n"
	"constant kV17st_72Mod := 0x00000020;\n"
	"constant kV29_96Mod := 0x00000040;\n"
	"constant kV17_96Mod := 0x00000080;\n"
	"constant kV17st_96Mod := 0x00000100;\n"
	"constant kV17_12Mod := 0x00000200;\n"
	"constant kV17st_12Mod := 0x00000400;\n"
	"constant kV17_14Mod := 0x00000800;\n"
	"constant kV17st_14Mod := 0x00001000;\n"
	"constant kCMOModemConnectSpeed := \"mspd\";\n"
	"constant kCMOModemFaxCapabilities := \"mfax\";\n"
	"constant kCMOSerialHWChipLoc := \"schp\";\n"
	"constant kHWLocExternalSerial := \"extr\";\n"
	"constant kHWLocBuiltInIR := \"infr\";\n"
	"constant kHWLocBuiltInModem := \"mdem\";\n"
	"constant kHWLocPCMCIASlot1 := \"slt1\";\n"
	"constant kHWLocPCMCIASlot2 := \"slt2\";\n"
	"constant kHWLocPCMCIAAnySlot := \"slot\";\n"
	"constant kCMOSlowIRConnect := \"irco\";\n"
	"constant kCMOSerialCircuitControl := \"sctl\";\n"
	"constant kCMOSerialChipSpec := \"sers\";\n"
	"constant kHMOSerExtClockDivide := \"cdiv\";\n"
	"constant kSerCap_Parity_Space := 0x00000001;\n"
	"constant kSerCap_Parity_Mark := 0x00000002;\n"
	"constant kSerCap_Parity_Odd := 0x00000004;\n"
	"constant kSerCap_Parity_Even := 0x00000008;\n"
	"constant kSerCap_DataBits_5 := 0x00000001;\n"
	"constant kSerCap_DataBits_6 := 0x00000002;\n"
	"constant kSerCap_DataBits_7 := 0x00000004;\n"
	"constant kSerCap_DataBits_8 := 0x00000008;\n"
	"constant kSerCap_StopBits_1 := 0x00000010;\n"
	"constant kSerCap_StopBits_1_5 := 0x00000020;\n"
	"constant kSerCap_StopBits_2 := 0x00000040;\n"
	"constant kSerCap_StopBits_All := 0x00000070;\n"
	"constant kSerCap_DataBits_All := 0x0000000f;\n"
	"constant kSerialChipUnknown := 0xff;\n"
	"constant kSerialChip8250 := 0x00;\n"
	"constant kSerialChip16450 := 0x01;\n"
	"constant kSerialChip16550 := 0x02;\n"
	"constant kSerialChip8530 := 0x20;\n"
	"constant kSerialChip6850 := 0x21;\n"
	"constant kSerialChip6402 := 0x22;\n"
	"constant kSerialChipVoyager := 0x23;\n"
	"constant kSerOutDTR := 0x01;\n"
	"constant kSerOutRTS := 0x02;\n"
	"constant kSerInDSR := 0x02;\n"
	"constant kSerInDCD := 0x08;\n"
	"constant kSerInRI := 0x10;\n"
	"constant kSerInCTS := 0x20;\n"
	"constant kSerInBreak := 0x80;\n"
	"constant kSerClk_Default := 0x00;\n"
	"constant kSerClk_DivideBy_1 := 0x80;\n"
	"constant kSerClk_DivideBy_16 := 0x81;\n"
	"constant kSerClk_DivideBy_32 := 0x82;\n"
	"constant kSerClk_DivideBy_64 := 0x83;\n"
	"constant kSerClk_DivideBy_Mask := 0x03;\n"
	"constant kUsingNegotiateIR := 0;\n"
	"constant kUsingSharpIR := 1;\n"
	"constant kUsingNewton1 := 2;\n"
	"constant kUsingNewton2 := 4;\n"
	"constant kUsing9600 := 1;\n"
	"constant kUsing19200 := 2;\n"
	"constant kUsing38400 := 4;\n"
	"constant kNormalConnect := 0;\n"
	"constant irSymmetricConnect := 1;\n"
	"constant irAcitveConnect := 2;\n"
	"constant kSnoozeAlert := 9;\n"
	"constant kSndBuffer := 1;\n"
	"constant kRcvBuffer := 2;\n"
	"constant kAtnBuffer := 3;\n"
	"constant editFont10 := 0x00002803;\n"
	"constant editFont12 := 0x00003003;\n"
	"constant editFont18 := 0x00004803;\n"
	"constant tsSystem := 0 << tsFamilyShift;\n"
	"constant tsHWFont := 3 << tsFamilyShift;\n"
	"constant kCMOAppletalkBuffer := \"bsiz\";\n"
	"constant kEOP := 0x00000000;\n"
	"constant kPacket := 0x00000002;\n"
	"constant kForever := 0x1fffffff;\n"
	"constant chNUL := 0;\n"
	"constant chSOH := 1;\n"
	"constant chSTX := 2;\n"
	"constant chETX := 3;\n"
	"constant chEOT := 4;\n"
	"constant chENQ := 5;\n"
	"constant chACK := 6;\n"
	"constant chBEL := 7;\n"
	"constant chBS := 8;\n"
	"constant chHT := 9;\n"
	"constant chLF := 10;\n"
	"constant chVT := 11;\n"
	"constant chFF := 12;\n"
	"constant chCR := 13;\n"
	"constant chSO := 14;\n"
	"constant chSI := 15;\n"
	"constant chDLE := 16;\n"
	"constant chDC1 := 17;\n"
	"constant chDC2 := 18;\n"
	"constant chDC3 := 19;\n"
	"constant chDC4 := 20;\n"
	"constant chNAK := 21;\n"
	"constant chSYN := 22;\n"
	"constant chETB := 23;\n"
	"constant chCAN := 24;\n"
	"constant chEM := 25;\n"
	"constant chSUB := 26;\n"
	"constant chESC := 27;\n"
	"constant chFS := 28;\n"
	"constant chGS := 29;\n"
	"constant chRS := 30;\n"
	"constant chUS := 31;\n"
	"constant kLocalPhoneDictionary := 112;\n"
	"constant kWorldPhoneDictionary := 114;\n"
	"constant kSimplePhoneDictionary := 115;\n"
	"constant kPostalCodeDictionary := 116;\n"
	"constant kNumbersOnlyDictionary := 117;\n"
	"constant kMoneyOnlyDictionary := 118;\n"
	"constant kSimpleDateDictionary := 119;\n"
	"constant kWorldPostalCodeDictionary := 120;\n"
	"constant kAddressAbbrevsDictionary := 121;\n"
	"constant kDraggerBorder := 7;\n"
	"constant kDraggerInset := 1;\n"
	"constant kDraggerBorderWidth := 8;\n"
	"constant kStdButtonFont := tsBold + ((9) << tsSizeShift);\n"
	"constant kNoTimeout := 0;\n"
	"constant kTimeOutImmediate := -1;\n"
	"constant kBarberIdle := 300;\n"
	"constant kFnCurrency := (1 << 4);\n"
	"constant kFnGrouping := (1 << 5);\n"
	"constant kFnNegParens := (1 << 6);\n"
	"constant kFnFixed := (1 << 7);\n"
	"constant kFnPercent := (1 << 8);\n"
	"constant kFnZapZeros := (1 << 9);\n"
	"constant kFnBasicCurrency := 242;\n"
	"constant vWidthGrowsWithText := 1 << 2;\n"
	"constant vFixedTextStyle := 1 << 3;\n"
	"constant vFixedInkTextStyle := 1 << 4;\n"
	"constant vExpectingNumbers := 1 << 9;\n"
	"constant vSingleKeystrokes := 1 << 10;\n"
	"constant vjParentAnchored := 256;\n"
	"constant vjFillCustom := 14;\n"
	"constant vjFrameCustom := 224;\n"
	"constant vjLinesCustom := 57344;\n"
	"constant vfDragger := 13;\n"
	"constant vfFrameDragger := vfBlack << vfFrameShift;\n"
	"constant kSoundCompleted := 0;\n"
	"constant kSoundAborted := 1;\n"
	"constant kSoundPaused := 2;\n"
	"constant firstXY := 6;\n"
	"constant lastXY := 7;\n"
	"constant finalXY := 8;\n"
	"constant kTextCompander := \"TLZStoreCompander\";\n"
	"constant kBitmapCompander := \"TPixelMapCompander\";\n"
	"constant kInkChar := $\\uF700;\n"
	"constant kNoOptions := 0;\n"
	"constant kHasScrollersOption := 1 << 0;\n"
	"constant kstdButtonSpacing := 7;\n"
	"func ButtonBounds(width)\n"
	"begin\n"
	"    if (width > 0) then\n"
	"        SetBounds(7, kstdButtonTop, 7 + width, kstdButtonBottom);\n"
	"    else\n"
	"        SetBounds(kfirstButtonLeft, kstdButtonTop, kfirstButtonLeft - width, kstdButtonBottom);\n"
	"end;\n"
	"func StdButtonWidth(str) MeasureString(str, tsSize(9)+tsBold)+6;\n"
	"constant kGestaltArg_HasBackLight := '[0x02000007, [struct,boolean], 1];\n"
	"constant kGestaltArg_HasSoftContrast := '[0x02000003, [struct, boolean, long, long], 1];\n"
	"constant kGestaltArg_VolumeInfo := '[0x02000009, [struct, boolean, boolean, boolean, boolean, real, long, long], 1];\n"
	"constant kGestaltArg_Microphone := '[0x0200000c, [struct, boolean], 1];\n"
	"constant kGestaltArg_BuiltIn_Keyboard := '[0x0200000b, [struct, boolean], 1];\n"
	"constant kNegativeZero := MakeBinaryFromHex(\"8000000000000000\", 'real);\n"
	"constant kInfinity := MakeBinaryFromHex(\"7FF0000000000000\", 'real);\n"
	"constant kNegativeInfinity := MakeBinaryFromHex(\"FFF0000000000000\", 'real);\n"
	"constant kNAN := MakeBinaryFromHex(\"7ff8000000000000\", 'real);\n"
	"constant opInProgress := 1;\n"
	"constant opSuccess := 0;\n"
	"constant opFailure := -1;\n"
	"constant opPartSuccess := -2;\n"
	"constant opReadOnly := -3;\n"
	"constant opNotSupported := -4;\n"
	"constant opBadOpCode := -5;\n"
	"constant opNotFound := -6;\n"
	"constant opTruncated := -7;\n"
	"constant opPadded := -8;\n"
	"constant kPortrait := 0;\n"
	"constant kLandscape := 1;\n"
	"constant kPortraitFlip := 2;\n"
	"constant kLandscapeFlip := 3;\n"
	"constant kDefaultDevice := 0x00;\n"
	"constant kInternalSpeaker := 0x01;\n"
	"constant kInternalModem := 0x02;\n"
	"constant kInternalMic := 0x04;\n"
	"constant kLineOut := 0x08;\n"
	"constant kLineIn := 0x10;\n"
	"constant kSampleStandard := 0;\n"
	"constant kSampleMuLaw := 1;\n"
	"constant kSampleLinear := 6;\n"
	"constant kKbdUsesKeyCodes := 1;\n"
	"constant kKbdTracksCaret := 2;\n"
	"constant kKbdforInput := 4;\n"
	"constant vStandAloneBounds := (1 << 11);\n"
	"constant vAlwaysTryKeyCommands := (1 << 12);\n"
	"constant vCallStandardScripts := (1 << 13);\n"
	"constant vTakesCommandKeys := (1 << 14);\n"
	"constant vTakesAllKeys := (1 << 15);\n"
	"constant vTakesNoKeys := (1 << 16);\n"
	"constant kWorksInAllModals := (1 << 0);\n"
	"constant kWorksInAppModals := (1 << 1);\n"
	"constant kRepeatable := (1 << 2);\n"
	"constant kTabKey := $\\09;\n"
	"constant kBackspaceKey := $\\08;\n"
	"constant kReturnKey := $\\0D;\n"
	"constant kEnterKey := $\\03;\n"
	"constant kEscKey := $\\1B;\n"
	"constant kLeftArrowKey := $\\1C;\n"
	"constant kRightArrowKey := $\\1D;\n"
	"constant kUpArrowKey := $\\1E;\n"
	"constant kDownArrowKey := $\\1F;\n"
	"constant kF1Key := $\\uF721;\n"
	"constant kF2Key := $\\uF722;\n"
	"constant kF3Key := $\\uF723;\n"
	"constant kF4Key := $\\uF724;\n"
	"constant kF5Key := $\\uF725;\n"
	"constant kF6Key := $\\uF726;\n"
	"constant kF7Key := $\\uF727;\n"
	"constant kF8Key := $\\uF728;\n"
	"constant kF9Key := $\\uF729;\n"
	"constant kF10Key := $\\uF72A;\n"
	"constant kF11Key := $\\uF72B;\n"
	"constant kF12Key := $\\uF72C;\n"
	"constant kF13Key := $\\uF72D;\n"
	"constant kF14Key := $\\uF72E;\n"
	"constant kF15Key := $\\uF72F;\n"
	"constant kIsSoftKeyboard := (1 << 24);\n"
	"constant kCommandModifier := (1 << 25);\n"
	"constant kShiftModifier := (1 << 26);\n"
	"constant kCapsLockModifier := (1 << 27);\n"
	"constant kOptionModifier := (1 << 28);\n"
	"constant kControlModifier := (1 << 29);\n"
	"constant kModifiersMask := 0x3E000000;\n"
	"constant kNoModifiers := 0;\n"
	"constant kRGB_16GrayIncrement := 0x00111111;\n"
	"constant kRGBpackedInteger := 0x10000000;\n"
	"constant kRGB_Black := kRGBpackedInteger + 0x00000000;\n"
	"constant kRGB_White := kRGBpackedInteger + 0x00FFFFFF;\n"
	"constant kRGB_Gray0 := kRGBpackedInteger + 0x00FFFFFF;\n"
	"constant kRGB_Gray1 := kRGBpackedInteger + 0x00EEEEEE;\n"
	"constant kRGB_Gray2 := kRGBpackedInteger + 0x00DDDDDD;\n"
	"constant kRGB_Gray3 := kRGBpackedInteger + 0x00CCCCCC;\n"
	"constant kRGB_Gray4 := kRGBpackedInteger + 0x00BBBBBB;\n"
	"constant kRGB_Gray5 := kRGBpackedInteger + 0x00AAAAAA;\n"
	"constant kRGB_Gray6 := kRGBpackedInteger + 0x00999999;\n"
	"constant kRGB_Gray7 := kRGBpackedInteger + 0x00888888;\n"
	"constant kRGB_Gray8 := kRGBpackedInteger + 0x00777777;\n"
	"constant kRGB_Gray9 := kRGBpackedInteger + 0x00666666;\n"
	"constant kRGB_Gray10 := kRGBpackedInteger + 0x00555555;\n"
	"constant kRGB_Gray11 := kRGBpackedInteger + 0x00444444;\n"
	"constant kRGB_Gray12 := kRGBpackedInteger + 0x00333333;\n"
	"constant kRGB_Gray13 := kRGBpackedInteger + 0x00222222;\n"
	"constant kRGB_Gray14 := kRGBpackedInteger + 0x00111111;\n"
	"constant kRGB_Gray15 := kRGBpackedInteger + 0x00000000;\n"
	"constant kCMSIrDA := \"irda\";\n"
	"constant kCMOIrDADiscoveryInformation := \"irdi\";\n"
	"constant kCMOIrDAConnectInformation := \"irci\";\n"
	"constant kCMOIrDAReceiveBuffers := \"irrb\";\n"
	"constant kCMOIrDALinkDisconnectTimeout := \"irld\";\n"
	"constant kCMOIrDAConnectUserData := \"ircd\";\n"
	"constant kCMOIrDAConnectAttrName := \"irca\";\n"
	"constant kCMOPCMCIAModemSound := \"msnd\";\n"
	"constant kIrDA1ProbeSlot := 1;\n"
	"constant kIrDA6ProbeSlot := 6;\n"
	"constant kIrDA8ProbeSlot := 8;\n"
	"constant kIrDA12ProbeSlot := 12;\n"
	"constant kIrDASvcHintPnPCompatible := 1;\n"
	"constant kIrDASvcHintPDAPalmtop := 2;\n"
	"constant kIrDASvcHintComputer := 4;\n"
	"constant kIrDASvcHintPrinter := 8;\n"
	"constant kIrDASvcHintModem := 16;\n"
	"constant kIrDASvcHintFAX := 32;\n"
	"constant kIrDASvcHintLanAccess := 64;\n"
	"constant kIrDASvcHintTelephony := 256;\n"
	"constant kIrDASvcHintFileServer := 512;\n";

const char* TToolkitPrototype::BytecodeDefs = "global bc := {\n"
											  // generate a 1-byte or 3-byte bytecode instruction from two integers
											  "    MakeBytecode: func(a, b)\n"
											  "        if b>6 then\n"
											  "          return bOr( bOr( bAnd(a<<19, 0xF80000), 0x070000 ), bAnd(b, 0xFFFF) )\n"
											  "        else\n"
											  "          return bOr( bAnd(a<<19, 0xF80000), bAnd(b<<16, 0x070000) ),\n"
											  // generate a 3-byte bytecode instruction from two integers
											  "    MakeBytecodePC: func(a, pc)\n"
											  "        return bOr( bOr( bAnd(a<<19, 0xF80000), 0x070000 ), bAnd(pc, 0xFFFF) ),\n"
											  "    Pop:            func() return :MakeBytecode(0, 0),\n"
											  "    Dup:            func() return :MakeBytecode(0, 1),\n"
											  "    Ret:            func() return :MakeBytecode(0, 2),\n"
											  "    PushSelf:       func() return :MakeBytecode(0, 3),\n"
											  "    SetLexScope:    func() return :MakeBytecode(0, 4),\n"
											  "    IterNext:       func() return :MakeBytecode(0, 5),\n"
											  "    IterDone:       func() return :MakeBytecode(0, 6),\n"
											  "    PopHandlers:    func() return :MakeBytecode(0, 7),\n"
											  "    Push:           func(lit)   return :MakeBytecode(3, lit),\n"
											  "    PushConst:      func(sImm)  return :MakeBytecode(4, sImm),\n" // FIXME: signed immediate!
											  "    CallGlobal:     func(nArgs) return :MakeBytecode(5, nArgs),\n"
											  "    Invoke:         func(nArgs) return :MakeBytecode(6, nArgs),\n"
											  "    Send:           func(nArgs) return :MakeBytecode(7, nArgs),\n"
											  "    SendIfDef:      func(nArgs) return :MakeBytecode(8, nArgs),\n"
											  "    Resend:         func(nArgs) return :MakeBytecode(9, nArgs),\n"
											  "    ResendIfDef:    func(nArgs) return :MakeBytecode(10, nArgs),\n"
											  "    Branch:         func(pc)    if IsSymbol(pc) then return [:MakeBytecodePC(11, 0), pc] else return :MakeBytecodePC(11, pc),\n"
											  "    BranchIfTrue:   func(pc)    if IsSymbol(pc) then return [:MakeBytecodePC(12, 0), pc] else return :MakeBytecodePC(12, pc),\n"
											  "    BranchIfFalse:  func(pc)    if IsSymbol(pc) then return [:MakeBytecodePC(13, 0), pc] else return :MakeBytecodePC(13, pc),\n"
											  "    FindVar:        func(lit)   return :MakeBytecode(14, lit),\n"
											  "    GetVar:         func(loc)   return :MakeBytecode(15, loc),\n"
											  "    MakeFrame:      func(nSlts) return :MakeBytecode(16, nSlts),\n"
											  "    MakeArray:      func(nVals) return :MakeBytecode(17, nVals),\n"
											  "    GetPath:        func(flag)  return :MakeBytecode(18, flag),\n"
											  "    SetPath:        func(flag)  return :MakeBytecode(19, flag),\n"
											  "    SetVar:         func(loc)   return :MakeBytecode(20, loc),\n"
											  "    FindAndSetVar:  func(lit)   return :MakeBytecode(21, lit),\n"
											  "    IncrVar:        func(loc)   return :MakeBytecode(22, loc),\n"
											  "    BranchIfNotDone:func(pc)    if IsSymbol(pc) then return [:MakeBytecodePC(23, 0), pc] else return :MakeBytecodePC(23, pc),\n"
											  "    Add:            func()  return :MakeBytecode(24, 0),\n"
											  "    Subtract:       func()  return :MakeBytecode(24, 1),\n"
											  "    ARef:           func()  return :MakeBytecode(24, 2),\n"
											  "    SetARef:        func()  return :MakeBytecode(24, 3),\n"
											  "    Equals:         func()  return :MakeBytecode(24, 4),\n"
											  "    LNot:           func()  return :MakeBytecode(24, 5),\n" // logic
											  "    NotEquals:      func()  return :MakeBytecode(24, 6),\n"
											  "    Multiply:       func()  return :MakeBytecode(24, 7),\n"
											  "    Divide:         func()  return :MakeBytecode(24, 8),\n"
											  "    DivInt:         func()  return :MakeBytecode(24, 9),\n"
											  "    Less:           func()  return :MakeBytecode(24, 10),\n"
											  "    Greater:        func()  return :MakeBytecode(24, 11),\n"
											  "    GreaterOrEqual: func()  return :MakeBytecode(24, 12),\n"
											  "    LessOrEqual:    func()  return :MakeBytecode(24, 13),\n"
											  "    BAnd:           func()  return :MakeBytecode(24, 14),\n"
											  "    BOr:            func()  return :MakeBytecode(24, 15),\n"
											  "    BNot:           func()  return :MakeBytecode(24, 16),\n"
											  "    NewIter:        func()  return :MakeBytecode(24, 17),\n"
											  "    Length:         func()  return :MakeBytecode(24, 18),\n"
											  "    Clone:          func()  return :MakeBytecode(24, 19),\n"
											  "    SetClass:       func()  return :MakeBytecode(24, 20),\n"
											  "    AddSlot:        func()  return :MakeBytecode(24, 21),\n"
											  "    Stringer:       func()  return :MakeBytecode(24, 22),\n"
											  "    HasPath:        func()  return :MakeBytecode(24, 23),\n"
											  "    ClassOf:        func()  return :MakeBytecode(24, 24),\n"
											  "    NewHandlers:    func(nEx)   return :MakeBytecode(25, nEx),\n"
											  "};\n";

//    constant kClassSymbol := kAppSymbol ;  '|Llama:NEWTONDTS|
//    constant kUserSoupName := kAppName ;   "Llama:NEWTONDTS"

const char* TToolkitPrototype::ToolkitDefs = "global newt := {\n"
											 "    writePkg: func() begin\n"
											 "       local pkg := MakePkg(app);\n"
											 "       SaveBinary(pkg, pkgPath);\n"
											 "    end,\n"
											 "    defaultIcon: {\n"
											 "        mask: MakeBinaryFromHex(\"000000000004000000000000001B0018000000000000000000001C0000003F0000003F001FFF7F003FFF7E003FFFFE003FFFFC003FFFFC003FFFF8003FFFF8003FFFF0003FFFF0003FFFE0003FFFE0003FFFC0003FFFC0003FFF80003FFF00003FFF80003FFF80003FFF80001FFF00001FFF00000FFE000000000000\", 'mask),\n"
											 "        bits: MakeBinaryFromHex(\"000000000004000000000000001B0018000000000000000000001C0000003F00000033001FFF7B003FFF6E003000CE0037FCCC0037FD9C0034059800355338003403300035567000340660003554E000340CC000354FC000340B8000360F000037FE800037FD8000300B8000180300001FFF00000FFE000000000000\", 'bits),\n"
											 "        bounds: {\n"
											 "            left: 0, top: 0, right: 24, bottom: 27\n"
											 "        }\n"
											 "    },\n"
											 "};\n"
											 // newt/64 will write all text that would otherwise go to stderr and stdout into these strings instead
											 "global _STDERR_ := \"\";\n"
											 "global _STDOUT_ := \"\";\n";

const char* TToolkitPrototype::DefaultPackage = "newt.theForm := NIL;\n"
												"newt.app := {\n"
												"    class: 'PackageHeader,\n"
												"    type: 2021161080,\n"
												"    pkg_version: 0,\n"
												"    version: 1,\n"
												"    copyright: \"Generated by the Einsten Toolkit.\",\n"
												"    name: \"Hello:WONKO\",\n"
												"    flags: 0,\n"
												"    parts: [\n"
												"        {\n"
												"            class: 'PackagePart,\n"
												"            info: MakeBinaryFromHex(\"41204E6577746F6E20546F6F6C6B6974206170706C69636174696F6E\", 'binary),\n"
												"            flags: 129,\n"
												"            type: 1718579821,\n"
												"            align: 8,\n"
												"            data: {\n"
												"                app: '|Hello:WONKO|,\n"
												"                text: \"Hello\",\n"
												"                icon: newt.defaultIcon,\n"
												"                theForm: newt.theForm\n"
												"            },\n"
												"        }\n"
												"    ]\n"
												"};\n";

const char* TToolkitPrototype::ToolkitLaunch =
	// catch execptions that happen while compiling the code
	"try begin\n"
	"#line 0\n";

const char* TToolkitPrototype::ToolkitDone = ";\n"
											 "end\n"
											 "onexception |evt.ex| do begin\n"
											 "    ex := CurrentException();\n"
											 "    print(\"Exception \");\n"
											 "    print(ex.name);\n"
											 "    if HasSlot(ex, 'data) then begin\n"
											 "        if HasSlot(ex.data, 'errorCode) then begin\n"
											 "            print(\": \");\n"
											 "            print(ex.data.errorCode);\n"
											 "        end;\n"
											 "        if HasSlot(ex.data, 'errorMessage) then begin\n"
											 "            print(\": \");\n"
											 "            print(ex.data.errorMessage);\n"
											 "        end;\n"
											 "        if HasSlot(ex.data, 'value) then begin\n"
											 "            print(\": \");\n"
											 "            print(ex.data.value);\n"
											 "        end;\n"
											 "    end;\n"
											 "    print(\"\\n\");\n"
											 "end;\n"
											 "if newt.theForm<>NIL then newt.app.parts[0].data.theForm := newt.theForm;\n"
											 "if kAppSymbol exists then newt.app.parts[0].data.app := kAppSymbol;\n"
											 "if kAppName exists then newt.app.name := kAppName;\n"
											 "if kAppLabel exists then newt.app.parts[0].data.text := kAppLabel;\n";

// GetVariable(GetRoot().|Hello:WONKO|, '_open) -> FOpenX -> RealOpenX__FRC6RefVarUc
// Sends aeShow to realDoCommand -> TView::Show() -> viewShowScript

NewtErrorText gNewtErrorTextList[] = {
	{ -7000, "Not enough memory available" },
	{ -8000, "@Application Error" },
	{ -8001, "PCMCIA card battery must be replaced" },
	{ -8002, "PCMCIA card battery is running low" },
	{ -8003, "Nothing to undo" },
	{ -8004, "The routing slip is already open" },
	{ -8005, "Close box must be tapped to hang up the modem" },
	{ -8006, "Nothing to print" },
	{ -8007, "Exception not handled" },
	{ -8008, "The length of a styles slot had to be extended" },
	{ -8009, "A length in the read-only styles slot is too short to display the text" },
	{ -8010, "Communications card has been inserted" },
	{ -8011, "Note has too many items" },
	{ -8012, "Note is too large" },
	{ -8013, "Note is too long" },
	{ -8100, "Blank note could not be created" },
	{ -8101, "Item could not be moved" },
	{ -8102, "Changes could not be saved" },
	{ -8103, "A problem has occurred" },
	{ -8104, "Problem with the PCMCIA card" },
	{ -8105, "Note could not be changed" },
	{ -8301, "@I/O Box Error" },
	{ -8301, "Missing transport" },
	{ -8302, "Missing slip" },
	{ -8303, "Cannot convert" },
	{ -8501, "@View System Error" },
	{ -8501, "Could not create view" },
	{ -8502, "Missing class slot" },
	{ -8503, "Unknown view stationery" },
	{ -8504, "Missing view flags" },
	{ -8505, "Missing view bounds" },
	{ -8601, "@State Machine Error" },
	{ -8601, "Invalid state" },
	{ -8602, "No state" },
	{ -8603, "No wait state" },
	{ -8604, "No polling routine" },
	{ -8605, "Polling timed out" },
	{ -8606, "Aborted" },
	{ -8607, "No reentrance" },
	{ -8608, "Invalid mode" },
	{ -10000, "@Operating System Error" },
	{ -10000, "Bad domain object ID" },
	{ -10001, "Bad physical page object ID" },
	{ -10002, "Unexpected object type" },
	{ -10003, "No page table" },
	{ -10004, "Allocation on an uninitialized heap" },
	{ -10005, "Call not implemented" },
	{ -10006, "Bad parameters" },
	{ -10007, "Not enough memory" },
	{ -10008, "Item not found" },
	{ -10009, "Could not create object" },
	{ -10010, "Must use a remote procedure call" },
	{ -10011, "Bad object" },
	{ -10012, "Not a user call" },
	{ -10013, "Task does not exist" },
	{ -10014, "Unexpected end of message" },
	{ -10015, "Bad object ID" },
	{ -10016, "Bad message object ID" },
	{ -10017, "Message already posted" },
	{ -10018, "Cannot cash token" },
	{ -10019, "Port no longer exists" },
	{ -10020, "No message waiting" },
	{ -10021, "Communications problem (message timed out)" },
	{ -10022, "Bad semaphore group ID" },
	{ -10023, "Bad semaphore operation list ID" },
	{ -10024, "Semaphore group no longer exists" },
	{ -10025, "Semaphore would cause blocking" },
	{ -10026, "Task no longer exists" },
	{ -10027, "Task aborted" },
	{ -10028, "Cannot suspend blocked task" },
	{ -10029, "Bad register number" },
	{ -10030, "Bad monitor function" },
	{ -10031, "No such monitor" },
	{ -10032, "Not a monitor" },
	{ -10033, "Size too large in shared memory call" },
	{ -10034, "Shared memory mode violation" },
	{ -10035, "Object not owned by task" },
	{ -10036, "Object not assigned to task" },
	{ -10037, "Total confusion" },
	{ -10038, "Another task already blocking" },
	{ -10039, "Cancelled" },
	{ -10040, "Object already initialized" },
	{ -10041, "Nested collection" },
	{ -10042, "Shared memory message no longer exists" },
	{ -10043, "Receiver did not perform remote procedure call" },
	{ -10044, "Copy aborted" },
	{ -10045, "Bad signature" },
	{ -10046, "Call not in progress" },
	{ -10047, "Token expected" },
	{ -10048, "Receiver object no longer exists" },
	{ -10049, "Monitor is not suspended" },
	{ -10050, "Not a fault monitor" },
	{ -10051, "No available page" },
	{ -10052, "Interrupt not enabled" },
	{ -10053, "Interrupt not implemented" },
	{ -10054, "Tric interrupt not enabled" },
	{ -10055, "Tric interrupt not implemented" },
	{ -10056, "Unresolved fault" },
	{ -10057, "Call already in progress" },
	{ -10058, "Offset beyond data" },
	{ -10059, "Bus access" },
	{ -10060, "Access permission" },
	{ -10061, "Permission violation" },
	{ -10062, "Duplicate object" },
	{ -10063, "Ill formed domain" },
	{ -10064, "Out of domains" },
	{ -10065, "Write protected" },
	{ -10066, "Timer expired" },
	{ -10067, "Not registered" },
	{ -10068, "Already registered" },
	{ -10069, "System restarted due to a power fault" },
	{ -10070, "System restarted because the battery was dead" },
	{ -10072, "System restarted because a PCMCIA card was removed while in use." },
	{ -10073, "RAM table is full" },
	{ -10074, "Unable to satisfy request" },
	{ -10075, "System error" },
	{ -10076, "System failure" },
	{ -10077, "New system software" },
	{ -10078, "Resource is claimed" },
	{ -10079, "Resource is unclaimed" },
	{ -10200, "@Stack Error" },
	{ -10200, "Stack too small" },
	{ -10201, "No room for heap" },
	{ -10202, "Stack is corrupted" },
	{ -10203, "Stack overflow" },
	{ -10204, "Stack underflow" },
	{ -10205, "Address out of range" },
	{ -10206, "Bad domain" },
	{ -10401, "@Package Error" },
	{ -10401, "Bad package" },
	{ -10402, "Package already exists" },
	{ -10403, "Bad package version" },
	{ -10404, "Unexpected end of package" },
	{ -10405, "Unexpected end of package part" },
	{ -10406, "Part type is already registered" },
	{ -10407, "Part type is not registered" },
	{ -10408, "No such package exists" },
	{ -10409, "Newer package already exists" },
	{ -10410, "Newer version of application already installed" },
	{ -10501, "@PCMCIA Card Error" },
	{ -10501, "Unrecognized card" },
	{ -10502, "Card not ready" },
	{ -10503, "Bad power on card" },
	{ -10504, "Unexpected card error" },
	{ -10505, "Card reset" },
	{ -10506, "Card is not initialized" },
	{ -10507, "Card service is not installed" },
	{ -10508, "Card service is not suspended" },
	{ -10509, "Card service has not been resumed" },
	{ -10510, "No usable configurations on card" },
	{ -10511, "Card could not be formatted" },
	{ -10512, "Card could not be formatted because it is write-protected" },
	{ -10520, "Bad CIS parser procedure pointer" },
	{ -10521, "Unknown tuple in CIS" },
	{ -10522, "Unknown subtuple in CIS" },
	{ -10523, "CIS tuple order is bad" },
	{ -10524, "CIS tuple size is bad" },
	{ -10525, "CIS tuple specified as no link has a link" },
	{ -10526, "CIS tuple specified with a link has no link" },
	{ -10527, "CIS tuple link target is bad" },
	{ -10528, "Bad CIS tuple version 1" },
	{ -10529, "Bad CIS tuple version 2" },
	{ -10530, "Bad CIS JEDEC tuple" },
	{ -10531, "Bad CIS checksum" },
	{ -10532, "Missing CIS" },
	{ -10533, "Blank CIS" },
	{ -10534, "Bad CIS" },
	{ -10535, "Bad link target" },
	{ -10551, "@Flash Card Error" },
	{ -10551, "Flash card is busy" },
	{ -10552, "Flash card is not erasing" },
	{ -10553, "Flash card erase is not suspended" },
	{ -10554, "Flash card suspend erase error" },
	{ -10555, "Flash card erase failed" },
	{ -10556, "Flash card write failed" },
	{ -10557, "Flash card Vpp is low" },
	{ -10558, "Flash card error in sleep" },
	{ -10559, "Flash card does not have enough power" },
	{ -10600, "@Card Store Error" },
	{ -10600, "Attempt to read or write outside of object bounds" },
	{ -10601, "Bad buffer pointer" },
	{ -10602, "Bad card access" },
	{ -10603, "Bad storage type" },
	{ -10604, "Store not found" },
	{ -10605, "The store has been write-protected by the user" },
	{ -10606, "Object not found" },
	{ -10607, "Flash card block is full" },
	{ -10608, "Flash card is not virgin" },
	{ -10609, "Write error (one or more bits failed to assert)" },
	{ -10610, "No more objects" },
	{ -10611, "Flash card erase in progress" },
	{ -10612, "Card is full" },
	{ -10613, "No more blocks left in search on flash card" },
	{ -10614, "Flash card log is full" },
	{ -10615, "Card needs to be formatted" },
	{ -10616, "Bad or unknown PSSID" },
	{ -10617, "Card memory is full" },
	{ -10618, "Missing or low battery on SRAM card" },
	{ -10619, "Attempt to modify store without a transaction in effect" },
	{ -10620, "Transaction aborted" },
	{ -10621, "Card needs recovery, but it is write-protected" },
	{ -10622, "Object too large for store" },
	{ -10800, "@DMA Error" },
	{ -10800, "DMA mode" },
	{ -10801, "DMA bus access" },
	{ -10802, "DMA buffer doesn't exist" },
	{ -10803, "DMA address word alignment" },
	{ -10804, "DMA count word alignment" },
	{ -10805, "DMA count size" },
	{ -10806, "DMA offset size" },
	{ -10820, "DMA PCMCIA ready" },
	{ -10821, "DMA PCMCIA input acknowledgment" },
	{ -10822, "DMA PCMCIA write protect" },
	{ -10823, "DMA PCMCIA time out" },
	{ -10900, "@Heap Error" },
	{ -10900, "Heap odd block size" },
	{ -10901, "Heap block out of range" },
	{ -10902, "Heap preferred free not found" },
	{ -10903, "Heap free accounting error" },
	{ -10904, "Heap accounting error" },
	{ -10905, "Heap block too big" },
	{ -10906, "Heap bad prior pointer" },
	{ -10907, "Heap bad last pointer in prior" },
	{ -10908, "Heap bad last pointer in last" },
	{ -12001, "@Generic AppleTalk Error" },
	{ -12001, "Buffer too small or corrupted" },
	{ -12002, "Event is pending" },
	{ -12003, "Cancelled" },
	{ -12004, "Attempt to cancel failed" },
	{ -12005, "No handler for cancel" },
	{ -12006, "Unknown message receiver" },
	{ -12007, "Cannot create AppleTalk port" },
	{ -12008, "Cannot create AppleTalk task" },
	{ -12009, "Not implemented" },
	{ -12010, "Data length error" },
	{ -12011, "No such subject available to open" },
	{ -12012, "Not opened" },
	{ -12013, "AppleTalk is not open" },
	{ -12014, "AppleTalk is already open" },
	{ -12015, "Duration is too small" },
	{ -12016, "Duration is too large" },
	{ -12100, "@LAP Protocol Error" },
	{ -12100, "LAP read link failed" },
	{ -12101, "LAP all protocols in use" },
	{ -12102, "No protocol handler" },
	{ -12103, "No such command" },
	{ -12104, "Bad link" },
	{ -12200, "@DDP Protocol Error" },
	{ -12200, "No such DDP command" },
	{ -12201, "Invalid socket" },
	{ -12202, "Not in static socket range" },
	{ -12203, "Not in dynamic socket range" },
	{ -12204, "Socket is already open" },
	{ -12205, "Socket not open" },
	{ -12206, "Socket internal socket" },
	{ -12207, "Socket is in use" },
	{ -12208, "Unknown LAP type" },
	{ -12209, "DDP back check sum" },
	{ -12210, "Bad packet size" },
	{ -12211, "No listener for socket" },
	{ -12212, "No such protocol type known" },
	{ -12213, "External client timed out" },
	{ -12300, "@NBP Protocol Error" },
	{ -12300, "Bad form" },
	{ -12301, "Name is already registered" },
	{ -12302, "Too many names" },
	{ -12303, "Name is not registered" },
	{ -12304, "Too many names requested" },
	{ -12305, "Too many lookups are pending" },
	{ -12306, "Not a NBP packet DDP type" },
	{ -12307, "Unknown NBP function" },
	{ -12308, "Unknown NBP lookup reply" },
	{ -12309, "Too many tuples in lookup request" },
	{ -12310, "NBP lookup is in use" },
	{ -12311, "NBP index out of range" },
	{ -12312, "NBP lookup aborted" },
	{ -12313, "No such command" },
	{ -12314, "No names found" },
	{ -12400, "@AEP Protocol Error" },
	{ -12400, "No such command" },
	{ -12401, "Not an echo packet DDP type" },
	{ -12402, "AEP packet size is zero" },
	{ -12403, "AEP function not requested" },
	{ -12500, "@RTMP Protocol Error" },
	{ -12500, "No such command" },
	{ -12502, "Packet size is zero" },
	{ -12503, "RTMP routed" },
	{ -12504, "RTMP address unresolved" },
	{ -12505, "RTMP no router available" },
	{ -12600, "@ATP Protocol Error" },
	{ -12600, "No such command" },
	{ -12601, "No ATP packet DDP type" },
	{ -12602, "Unknown ATP function" },
	{ -12603, "ATP request data length is zero" },
	{ -12604, "Expected responses are out of range" },
	{ -12605, "Response buffer is too small" },
	{ -12606, "ATP retry duration too small" },
	{ -12607, "ATP transaction timed out" },
	{ -12608, "Responding socket already open" },
	{ -12609, "Responding socket not open" },
	{ -12610, "Response packet length bad" },
	{ -12611, "Bad number of response packets" },
	{ -12612, "Socket already has a request on autorequest" },
	{ -12700, "@PAP Protocol Error" },
	{ -12700, "No such command" },
	{ -12701, "Unexpected connection ID" },
	{ -12702, "Invalid connection ID" },
	{ -12703, "Invalid responder socket" },
	{ -12704, "Unexpected function" },
	{ -12705, "Printer is busy" },
	{ -12706, "Unexpected connection open result" },
	{ -12707, "Bad flow quantum requested" },
	{ -12708, "Connection timed out" },
	{ -12709, "EOF sent" },
	{ -12710, "PAP flushed" },
	{ -12711, "Printer terminated connection" },
	{ -12712, "Printer not found" },
	{ -12713, "No status available" },
	{ -12714, "No data available" },
	{ -12715, "The buffer that was passed is too small" },
	{ -12716, "Put data operation timed out" },
	{ -12800, "@ZIP Protocol Error" },
	{ -12800, "No zones" },
	{ -12900, "@ADSP Protocol Error" },
	{ -12900, "Too many ADSP connections" },
	{ -12901, "ADSP mode invalid" },
	{ -12902, "ADSP packet size bad" },
	{ -12903, "ADSP control type bad" },
	{ -12904, "Remote end disconnected" },
	{ -14001, "@Utility Class Error" },
	{ -14001, "Not implemented" },
	{ -14002, "Out of memory" },
	{ -14003, "Bad position" },
	{ -14004, "Already initialized" },
	{ -14005, "Invalid size" },
	{ -14006, "Overflow" },
	{ -14007, "Underflow" },
	{ -14008, "Range check failed" },
	{ -14009, "Element sizes do not match" },
	{ -14010, "Not initialized" },
	{ -14011, "Pointer is nil" },
	{ -16001, "@Communications Tool Error" },
	{ -16001, "Command in progress" },
	{ -16002, "Bad command" },
	{ -16003, "Tool already has maximum requests pending" },
	{ -16004, "Buffer overflow" },
	{ -16005, "Request canceled by kill request" },
	{ -16006, "Bad parameter in request" },
	{ -16007, "Connection end has not been created yet" },
	{ -16008, "Invalid call when connected" },
	{ -16009, "Phone connection was cut off, or invalid call when not connected" },
	{ -16010, "Connection negotiation failed because remote end is not compatible with local end configuration" },
	{ -16011, "Connection terminated or failed due to retransmission limit of data or connect packet" },
	{ -16012, "No data available for TCommToolGetRequest when fNonBlocking is true" },
	{ -16013, "Connection aborted by disconnect" },
	{ -16014, "Call not supported" },
	{ -16015, "Request not pending" },
	{ -16016, "Event not pending" },
	{ -16017, "Time-out waiting for connection" },
	{ -16018, "Connection end is already bound" },
	{ -16019, "Connection end was not bound before use" },
	{ -16020, "Connection end is being released" },
	{ -16021, "No phone number was provided" },
	{ -16022, "Operation failed because a resource was not available" },
	{ -16023, "Call failed because the option passed is not supported" },
	{ -16024, "The method is not implemented" },
	{ -18000, "@Serial Tool Error" },
	{ -18000, "Serial channel is in use" },
	{ -18001, "Memory error" },
	{ -18002, "Not current owner of the serial port" },
	{ -18003, "Framing or parity overrun, or bad connection" },
	{ -18004, "CRC error on input framing" },
	{ -18005, "An internal error has occurred" },
	{ -18006, "Packet size too large or too small in an output request" },
	{ -18007, "Unexpected packet length" },
	{ -18008, "EOF not found" },
	{ -18009, "Overrun bit was set" },
	{ -18010, "Too many collisions when sending packet" },
	{ -18011, "Too many deferrals when sending packet" },
	{ -18012, "Timed out waiting for an event" },
	{ -18013, "Serial tool is not active or ready" },
	{ -20001, "@MNP Tool Error" },
	{ -20001, "Connection parameter negotiation failed" },
	{ -20002, "Acceptor of connect request timed out" },
	{ -20003, "Not connected" },
	{ -20004, "Request aborted by disconnect request" },
	{ -20005, "Link attention service is not enabled" },
	{ -20006, "Request retry limit of connect initiator reached" },
	{ -20007, "Command already in progress" },
	{ -20008, "Connection already established" },
	{ -20009, "Connection failed due to incompatible protocol levels" },
	{ -20010, "Connection handshake failed" },
	{ -20011, "Memory for MNP not allocated" },
	{ -22001, "@FAX Tool Error" },
	{ -22001, "Lost connection while sending or receiving FAX" },
	{ -22002, "FAX machine is not compatible" },
	{ -22003, "Transmission error" },
	{ -22005, "FAX machine had a problem sending some pages" },
	{ -22006, "Transmission error" },
	{ -22007, "Transmission error" },
	{ -24000, "@Modem Tool Error" },
	{ -24000, "No modem is connected" },
	{ -24001, "There is no dial tone" },
	{ -24002, "There is no answer" },
	{ -24003, "The phone number is busy" },
	{ -24004, "There is no answer" },
	{ -24005, "The modem is not responding properly" },
	{ -24006, "FAX carrier error" },
	{ -24007, "The modem is not responding properly" },
	{ -24008, "The modem connected to the serial port does not support cellular connection" },
	{ -24009, "The AT+FRH command timed out when receiving flags" },
	{ -26000, "@Communications Manager Error" },
	{ -26000, "Service already initialized" },
	{ -26001, "Unknown command" },
	{ -26002, "Unknown service" },
	{ -26003, "Service already exists" },
	{ -26004, "No service specified in the options array" },
	{ -26005, "There is no registered service matching the type specified in the options array" },
	{ -26006, "No endpoint exists; this is usually because CMStartService has not been called" },
	{ -26007, "No public port exists; this is usually because CMGetEndPoint has not been called" },
	{ -26008, "No known last connected device" },
	{ -26009, "A tuple has been received, but no the device ID tuple" },
	{ -26010, "A service information response tuple was expected" },
	{ -26011, "Unsupported service; can only load packages" },
	{ -26012, "An SCP load is in progress and another cannot be issued" },
	{ -26013, "The SCP load call is not supported on this machine" },
	{ -26014, "Cannot process this speed" },
	{ -26015, "The SCP loader did not previously load a package" },
	{ -28001, "@Docker Error" },
	{ -28001, "Invalid store signature" },
	{ -28002, "Invalid entry" },
	{ -28003, "Aborted" },
	{ -28004, "Invalid query" },
	{ -28005, "Read entry error" },
	{ -28006, "Invalid current soup" },
	{ -28007, "Invalid command length" },
	{ -28008, "Entry not found" },
	{ -28009, "Bad connection" },
	{ -28010, "File not found" },
	{ -28011, "Incompatible protocol" },
	{ -28012, "Protocol error" },
	{ -28013, "Docking canceled" },
	{ -28014, "Store not found" },
	{ -28015, "Soup not found" },
	{ -28016, "Invalid header" },
	{ -28017, "Out of memory" },
	{ -28018, "Newton version too new" },
	{ -28019, "Package cannot load" },
	{ -28020, "Protocol already registered" },
	{ -28021, "Remote import error" },
	{ -28022, "Bad password error" },
	{ -28023, "Password retry" },
	{ -28024, "Idle too long" },
	{ -28025, "Out of power" },
	{ -28026, "Invalid cursor" },
	{ -28027, "Already busy" },
	{ -28028, "Desktop error" },
	{ -28029, "Cannot connect to modem" },
	{ -28030, "Disconnected" },
	{ -28031, "Access denied" },
	{ -28100, "Disconnect during read" },
	{ -28101, "Read failed" },
	{ -28102, "Communications tool not found" },
	{ -28103, "Invalid modem tool version" },
	{ -28104, "Card not installed" },
	{ -28105, "Browser File Not Found" },
	{ -28106, "Browser Volume Not Found" },
	{ -28107, "Browser Path Not Found" },
	{ -28200, "@Docker Import and Export Error" },
	{ -28200, "Syntax error" },
	{ -28201, "Invalid version" },
	{ -28202, "Could not open temporary store" },
	{ -28203, "Could not convert" },
	{ -28204, "Invalid criteria" },
	{ -28205, "Error applying script" },
	{ -28206, "Missing meta data" },
	{ -28207, "Unknown error" },
	{ -28208, "Scanner overflow error" },
	{ -28209, "Data Viz translator error" },
	{ -28210, "Invalid type" },
	{ -28300, "@Docker Disk Error" },
	{ -28300, "Disk full" },
	{ -28301, "File not found" },
	{ -28302, "File is write protected" },
	{ -28303, "Duplicate file name" },
	{ -28304, "Too many files open" },
	{ -28700, "@Docker Desktop DIL Error" },
	{ -28700, "No Error" },
	{ -28701, "Out of memory" },
	{ -28702, "Invalid pipe state" },
	{ -28703, "Exception error" },
	{ -28704, "Queue full" },
	{ -28705, "Pipe not initialized" },
	{ -28706, "Invalid parameter" },
	{ -28707, "Pipe not ready" },
	{ -28800, "No Error" },
	{ -28801, "Out of object heap memory" },
	{ -28802, "Out of temporary memory" },
	{ -28803, "Unknown slot" },
	{ -28804, "Slot size exceeded" },
	{ -28805, "Slot size required" },
	{ -30000, "@Sound Error" },
	{ -30000, "Generic sound error" },
	{ -30001, "Not enough memory available" },
	{ -30002, "Invalid message" },
	{ -30003, "Sound was not played" },
	{ -30004, "No channel decompressor" },
	{ -30005, "Destination buffer too small" },
	{ -30006, "Sound player busy" },
	{ -30007, "Sound recorder busy" },
	{ -30008, "No samples provided" },
	{ -30009, "Unsupported sound configuration" },
	{ -30010, "Sound channel closed" },
	{ -30011, "Sound cancelled" },
	{ -30012, "The sound volume is set to zero" },
	{ -32001, "@Compression Error" },
	{ -32001, "Cannot compress in place" },
	{ -32002, "Parsing error" },
	{ -32003, "Invalid type" },
	{ -32004, "Compression not achieved" },
	{ -32005, "Key not found" },
	{ -32006, "Compression index error" },
	{ -32007, "Cannot decompress in place" },
	{ -32008, "Decompression not achieved" },
	{ -32009, "Unexpected end of source" },
	{ -32100, "Buffer overflow" },
	{ -32101, "Buffer underflow" },
	{ -34000, "@Memory Error" },
	{ -34000, "Not free, direct or indirect" },
	{ -34001, "Pointer not aligned to 4-byte boundary" },
	{ -34002, "Pointer to outside of heap" },
	{ -34003, "Unknown infrastructure type" },
	{ -34004, "Free block where there shouldn't be one" },
	{ -34005, "Free list pointer points outside of heap" },
	{ -34006, "Free-list pointer doesn't point at a free block" },
	{ -34007, "Invalid block size" },
	{ -34008, "Forbidden bits set in block size" },
	{ -34009, "Less than minimum size for heap block" },
	{ -34010, "Heap block tool large" },
	{ -34011, "Total free space is more than space for entire heap" },
	{ -34012, "Nil pointer where not allowed" },
	{ -34013, "Actual free space does not match tracked free space" },
	{ -34014, "Linked free space does not match tracked free space" },
	{ -34015, "Master pointer doesn't point back to a handle block" },
	{ -34016, "Invalid block size adjustment" },
	{ -34017, "Internal block may be mangled" },
	{ -34018, "The heap is invalid" },
	{ -34019, "Caught an exception while checking the heap" },
	{ -34020, "Invalid heap header" },
	{ -36001, "@Communications Transport Error" },
	{ -36001, "Incorrect address format" },
	{ -36002, "Incorrect option format" },
	{ -36003, "Incorrect permissions" },
	{ -36005, "Could not allocated address" },
	{ -36006, "Operation not supported in the current tool state" },
	{ -36008, "System error" },
	{ -36012, "Flow control problem" },
	{ -36018, "Unsupported primitive" },
	{ -36019, "State change is in process" },
	{ -36030, "There's already a synchronous call pending" },
	{ -38001, "@Sharp IR Error" },
	{ -38001, "No response - protocol time out" },
	{ -38002, "Cancelled - remote side cancelled operation" },
	{ -38003, "Protocol error" },
	{ -38004, "Data checksum failed" },
	{ -38005, "Remote side receive failed" },
	{ -38006, "Bad connection - allowed number of retries exceeded" },
	{ -38007, "SCC data errors on receive" },
	{ -38008, "Unspecified beaming error" },
	{ -40102, "@Online Service Error" },
	{ -40102, "Lost connection to host" },
	{ -40103, "Lost connection to host" },
	{ -40104, "The host is not responding" },
	{ -40105, "There is a problem reading from the host" },
	{ -40106, "Failed to connect to local access number" },
	{ -44000, "@Printing Error" },
	{ -44000, "Printer problem" },
	{ -44001, "Newton is unable to print" },
	{ -44002, "No printer is connected" },
	{ -44003, "Printer busy" },
	{ -44004, "Printing stopped" },
	{ -44005, "Lost contact with the printer" },
	{ -44006, "Image too complex for printer" },
	{ -44100, "The next sheet of paper must be inserted" },
	{ -44101, "The phone number must be dialed now" },
	{ -44102, "There is no paper tray" },
	{ -44103, "The wrong paper tray is attached" },
	{ -44104, "The printer has no paper" },
	{ -44105, "The printer has no ink" },
	{ -44106, "The printer is jammed" },
	{ -44107, "The printer door is open" },
	{ -44108, "The printer is off-line" },
	{ -46001, "@Newton Connection Error" },
	{ -46001, "Connection initialization failed" },
	{ -46002, "Timer error" },
	{ -46003, "Connection request was denied by the remote" },
	{ -46004, "Unable to connect because there are no endpoints available" },
	{ -46005, "A connect request was received but no service name was given" },
	{ -48001, "@Store and Soup Error" },
	{ -48001, "The PCMCIA card is not a data storage card" },
	{ -48002, "Store format is too old to understand" },
	{ -48003, "Store format is too new to understand" },
	{ -48004, "Store is corrupted, can't recover" },
	{ -48005, "Single object is corrupted, can't recover" },
	{ -48006, "Object stream has unknown format version" },
	{ -48007, "Fault block is invalid" },
	{ -48008, "Not a fault block" },
	{ -48009, "Not a soup entry" },
	{ -48010, "Tried to remove a store that was not registered" },
	{ -48011, "Soup index has an unknown type" },
	{ -48012, "Soup index has an unknown key structure" },
	{ -48013, "Soup index does not exist" },
	{ -48014, "A soup with this name already exists" },
	{ -48015, "Tried to CopyEntries? to a union soup" },
	{ -48016, "Soup is invalid (probably from a removed store)" },
	{ -48017, "Soup is invalid (probably from a removed store)" },
	{ -48018, "Entry is invalid (probably from a removed store)" },
	{ -48019, "Key does not have the type specified in the index" },
	{ -48020, "Store is in ROM" },
	{ -48021, "Soup already has an index with this path" },
	{ -48022, "Internal error-something unexpected happened" },
	{ -48023, "Tried to call RemoveIndex? on the _uniqueID index" },
	{ -48024, "Query type missing or unknown" },
	{ -48025, "Discovered index inconsistency" },
	{ -48026, "Maximum number of soup tags reached" },
	{ -48027, "Soup does not have a tags index" },
	{ -48028, "Invalid tags specification in the query" },
	{ -48029, "Store cannot handle the feature (for example, large objects)" },
	{ -48030, "Unknown sorting table" },
	{ -48031, "Cannot do union soup because of different sorting tables" },
	{ -48032, "Invalid index description" },
	{ -48033, "Cannot use virtual objects for soup entry keys" },
	{ -48200, "@Object System Error" },
	{ -48200, "Expected a frame, array, or binary object" },
	{ -48201, "Invalid magic pointer" },
	{ -48202, "Empty path" },
	{ -48203, "Invalid segment in path expression" },
	{ -48204, "Path failed" },
	{ -48205, "Index out of bounds (string or array)" },
	{ -48206, "Source and destination must be different objects" },
	{ -48207, "Long out of range" },
	{ -48210, "Bad arguments" },
	{ -48211, "String too big" },
	{ -48212, "Expected a frame, array, or binary object" },
	{ -48213, "Expected a frame, array, or binary object" },
	{ -48214, "Object is read-only" },
	{ -48216, "Out of heap memory" },
	{ -48217, "Invalid attempted use of magic pointer" },
	{ -48218, "Cannot create or change an object to negative size" },
	{ -48219, "Value out of range" },
	{ -48220, "Could not resize locked object" },
	{ -48221, "Reference to deactivated package" },
	{ -48222, "Exception is not a subexception of |evt.ex|" },
	{ -48400, "@Bad Type Error" },
	{ -48400, "Expected a frame" },
	{ -48401, "Expected an array" },
	{ -48402, "Expected a string" },
	{ -48403, "Expected a frame, array, or binary object" },
	{ -48404, "Expected a number" },
	{ -48405, "Expected a real" },
	{ -48406, "Expected an integer" },
	{ -48407, "Expected a character" },
	{ -48408, "Expected a binary object" },
	{ -48409, "Expected a path expression (or a symbol or integer)" },
	{ -48410, "Expected a symbol" },
	{ -48411, "Expected a function" },
	{ -48412, "Expected a frame or an array" },
	{ -48413, "Expected an array or nil" },
	{ -48414, "Expected a string or nil" },
	{ -48415, "Expected a binary object or nil" },
	{ -48416, "Unexpected frame" },
	{ -48417, "Unexpected binary object" },
	{ -48418, "Unexpected immediate" },
	{ -48419, "Expected an array or string" },
	{ -48420, "Expected a virtual binary object" },
	{ -48421, "Expected a package" },
	{ -48422, "Expected nil" },
	{ -48423, "Expected nil or a symbol" },
	{ -48424, "Expected nil or true" },
	{ -48425, "Expected an integer or an array" },
	{ -48600, "@Compiler Error" },
	{ -48600, "Could not open a listener window" },
	{ -48601, "Syntax error" },
	{ -48603, "Cannot assign to a constant" },
	{ -48604, "Cannot test for subscript existence; use length" },
	{ -48605, "Global variables not allowed in applications" },
	{ -48606, "Cannot have a global variable and a global constant with the same name" },
	{ -48607, "Cannot redefine a constant" },
	{ -48608, "Cannot have a variable and a constant with the same name in the same scope" },
	{ -48609, "Non-literal expression for constant initializer" },
	{ -48610, "End of input inside a string" },
	{ -48611, "Odd number of digits between \\u's" },
	{ -48612, "No escapes but \\u are allowed after \\u" },
	{ -48613, "Invalid hex character in \\u string" },
	{ -48617, "Two-digit hex number required after $\\ escape" },
	{ -48618, "Four-digit hex number required after $\\u" },
	{ -48619, "Illegal character '%c'" },
	{ -48620, "Invalid hexadecimal integer: %s (out of range)" },
	{ -48621, "Invalid real number (out of range)" },
	{ -48622, "Invalid decimal integer: %s (out of range)" },
	{ -48626, "#xxxx not allowed from NTK" },
	{ -48627, "Not a constant" },
	{ -48628, "Decimal digit required after @" },
	{ -48800, "@Interpreter Error" },
	{ -48800, "Not in a break loop" },
	{ -48803, "Wrong number of arguments" },
	{ -48804, "FOR loop BY expression has value zero" },
	{ -48806, "No current exception" },
	{ -48807, "Undefined variable" },
	{ -48808, "Undefined global function" },
	{ -48809, "Undefined method" },
	{ -48810, "No _proto for inherited send" },
	{ -48811, "Tried to access slot of nil" },
	{ -48814, "Local variables and FOR/WITH loops not allowed at top level" },
	{ -48815, "The operation would make the rich string invalid" },
	{ -54000, "@Communications Endpoint Error" },
	{ -54000, "No active input script" },
	{ -54001, "Badly formed script" },
	{ -54002, "Zero length data" },
	{ -54003, "Expected a specification" },
	{ -54004, "Invalid option" },
	{ -54005, "Invalid end sequence" },
	{ -54006, "Inappropriate partial" },
	{ -54007, "Inappropriate termination" },
	{ -54008, "Inappropriate target" },
	{ -54009, "Inappropriate filter" },
	{ -54010, "Expected target" },
	{ -54011, "Expected template" },
	{ -54012, "Input specification already active" },
	{ -54013, "Invalid proxy" },
	{ -54014, "No endpoint available" },
	{ -54015, "Inappropriate call" },
	{ -54016, "Character is not single byte" },
	{ -54021, "Option failure" },
	{ -54022, "Option partial success" },
	{ -54023, "Option read only" },
	{ -54024, "Option not supported" },
	{ -54025, "Invalid option op code" },
	{ -54026, "Option not found" },
	{ -54027, "Option truncated" },
	{ -56001, "@Tablet Driver Error" },
	{ -56001, "Attempted to call the tablet driver before it was loaded" },
	{ -56002, "Attempted to create a tablet driver a second time" },
	{ -56003, "Creation of tablet driver failed" },
	{ -56004, "Unable to enter bypass mode" },
	{ -56005, "Not in bypass mode" },
	{ -56006, "Cannot add sample to buffer" },
	{ -56007, "No new data since last polling time" },
	{ -56008, "Unsupported function" },
	{ -56101, "Timeout when calibrating" },
	{ -56102, "Calibration aborted" },
	{ -56201, "@Battery Driver Error" },
	{ -56201, "Could not find battery driver" },
	{ -56202, "Battery error" },
	{ -56203, "Invalid battery selector" },
	{ -58001, "@Alien Store Error" },
	{ -58001, "Oversize page" },
	{ -58002, "No such page" },
	{ -58003, "Cannot repage ID" },
	{ -58004, "No more for that page" },
	{ -58005, "Store is damaged" },
	{ 0, nullptr },
};

// ======================================================================= //
// Newton Toolkit
//
// A development environment created by Apple for writing NewtonScript programs
// for Newton OS. A prototype for Newton Toolkit called EtchASketch was written
// using Macintosh Common Lisp. Also referred to as NTK.
//
//   -- Newton Glossary
// ======================================================================= //
