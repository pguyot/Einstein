/*
	Install & Remove.f
*/

SetPartFrameSlot('DoNotInstall,
func()
begin
	if not GlobalFnExists('OpenNativeLibrary) then
	begin
		GetRoot():Notify(3, kAppName, "Sorry, this software requires Relativity." );
		return TRUE;
	end;
	return NIL;
end );
