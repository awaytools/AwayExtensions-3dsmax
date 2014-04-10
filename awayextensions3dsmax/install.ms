--01.) declare vars in RAM in order tot access them before they are initialized
AWDExtensions_version_nr = "1.0";--the version number

rl_install;
rl_uninstall;
rl_setup;
setup_AWDExtensions_uninstall_step1;
setup_AWDExtensions_install;
setup_AWDExtensions_quit;
setup_AWDExtensions_uninstall_getList;
setup_AWDExtensions_install_manually;

--02.) try to kill previous floaters
try (--close if possible
	DestroyDialog rl_install;
) catch();
try (--close if possible
	DestroyDialog rl_uninstall;
) catch();
try (--close if possible
	DestroyDialog rl_setup;
) catch();


function setup_AWDExtensions_update prct msg=(
	rl_install.pb_bar.value = prct;--0 - 100
	rl_install.txt_stats.text = msg as string;--update message
)
function getBitVersionAsString = (
	if (is64bitapplication()) then return "64"
	else return "32"
	)
function setup_AWDExtensions_install_manually = (
	
	local m =  "It appears that your temporary folder is not accessable\rto extract the files from the MZP file. Or there is no permission\rto copy files to the userDirectories. So if you keep\nseeing this message please install AWDExtensions manually with\rthe following steps:\r"
	m+="\n\n1.) Rename the AWDExtensions '.mzp' extention to '.zip' and open\rit with Winzip, Winrar or whatever."
	m+="\n\n2.) Copy the main plugin-file (maxawdexporter_201*_"+getBitVersionAsString()+".dle) into your 3dsmax-plugin-folder."
	m+="\ni.e.: "+"C:\Program Files\Autodesk\3ds Max 2013\plugins\ "
	m+="\n\n3.) Copy the folder 'AwayExtensions3dsMax' into your 3dsmax-plugin-folder (same target-folder as previous step)."
	m+="\n\n4.) Copy the content of the folder 'Icons' into your 3dsmax-usericon-folder "
	m+="\ni.e.: "+"C:\Users\80prozent\AppData\Local\Autodesk\3dsMax\2013 - 64bit\ENU\usericons\ "
	m+="\n\n5.) Copy the folder AWDCommands into your 3dsmax-usermacros-folder "
	m+="\ni.e.: "+"C:\Users\80prozent\AppData\Local\Autodesk\3dsMax\2013 - 64bit\ENU\usermacros\ "
	m+="\n\n6.) Open 3dsmax."
	m+="\n\n7.) In the Main Menu go to 'Customize' -> 'Customize User Interface'."
	m+="\n\n8.) In 3dsmax 2012 load the file 'AWDToolBar_2011_2012.CUI', in 3dsmax 2013/2014 load the file 'AWDToolBar_2013_2014.cuix'"
	m+="\n\n\nFor more help goto:\nhttp://www.away3d.com/forum"
	messageBox m title:"Manual Installation Guide" beep:false
)

function setup_AWDExtensions_find_MZP_url =(
	local url = sysInfo.tempdir as string;
	local folders = getDirectories (url+"mzptmp*");--temporay MZP folders
	
	local _mathchUrls = #();--store possible matches and have them sort by date later
	local _matchDates = #();
	local _pCnt = 0.0;
	for f in folders do (
		_pCnt+=1.0;
		--find the right folder and file from theh MZP archive
		
		--we could check for more directories, but no other mzp file should have a folder called "AwayExtensions3dsMax"
		local  has_folder= getDirectories (f+"AwayExtensions3dsMax");
		local  has_mzp= getFiles (f+"mzp.run");
		
		local p = _pCnt / folders.count* 18.0;
		
		if (has_folder.count == 1 and has_mzp.count == 1)then(
			local _date = getFileCreateDate has_mzp[1];
		
			append _matchDates _date;
			append _mathchUrls f;
		
		)
	)
	
	if (_mathchUrls.count > 0)then(
		--found some urls that match our footprint, now sort by date and pick the latest
		local _new = amax _matchDates;--the latest date item
		local _indx = findItem _matchDates _new;
		local final_url = _mathchUrls[_indx];
		return final_url;
	)else(
		return "";
	)
)
function setup_AWDExtensions_quit =(
	--clearlistener();
	print("Quitting the setup (deleting left over MZP files)");
	--delete temporary MZP files (3dsmax generated them, not this script)
	local final_url = setup_AWDExtensions_find_MZP_url();
	if (final_url != "")then(
		
		
		--generate a list of files to delete
		local del_folders =#();
		local del_files =#();
		
		local folders = getDirectories (final_url+"*");--temporay MZP folders
		for f in folders do (
			append del_folders f;
			
			local files = getFiles (f+"\\*");--delete files inside this folder before we can delete the folder
			for fl in files do (
				append del_files fl;
			)
			local files = getDirectories (f+"\\*");--delete files inside this folder before we can delete the folder
			for fldr in files do (
				local files2 = getFiles (fldr+"\\*");--delete files inside this folder before we can delete the folder
				for fl in files2 do (
					append del_files fl;
				)
			)
		)
		append del_folders final_url;
		----
		for s in del_files do (
			--files
			local _done = deleteFile s;
			--format ("\t"+s+", done: "+_done as string+"\n");
			--format ("\tdel f: "+s+", delete: \n");
		)
		for s in del_folders do (
			local str = "rmdir \""+s+"\"";
			local _done;
			hiddendoscommand str startpath:"$max" exitCode:&_done
			--format (str+"\t\tdone: "+_done as string+"\n");
			--format ("\tdel d: "+s+", delete: \n");
		)
	)
)
function setup_AWDExtensions_install =(
	--clearlistener();
	
	try (--close if possible the existing Toolbar interface
		DestroyDialog rl_AWDExtensions;
	) catch();

	setup_AWDExtensions_update 0 "init install";
	local final_url = setup_AWDExtensions_find_MZP_url();
	if (final_url != "")then(
		setup_AWDExtensions_update 20 "found MZP files";
		--prepare to copy files from /AwayExtensions3dsMax 
		local _scriptDir = pathConfig.GetDir #maxroot + "\\plugins\\AwayExtensions3dsMax";
		local accessFolder = makeDir _scriptDir;
		if (accessFolder == true)then(
			

			local failing = false;
			
			if ((maxVersion())[1] / 1000)==13 then(
				print (final_url+"maxawdexporter_2011_"+getBitVersionAsString()+".dle")
				local _scriptDir = pathConfig.GetDir #maxroot + "\\plugins";				
				local _filesSource = getFiles (final_url+"maxawdexporter_2011_"+getBitVersionAsString()+".dle");
				print 	(_filesSource as string)		
				for sf in _filesSource do (--source file
					local tf = _scriptDir+"\\"+filenameFromPath sf;--target file					
					local accessFile = deleteFile tf;--try to delete the icons if they can not be replaced below
					local accessFile = copyFile sf tf;
				)
				local _scriptDir = pathConfig.GetDir #ui;				
				local _filesSource = getFiles (final_url+"\\AWDToolBar_2011_2012.CUI");
				local _pCnt = 0.0;--percentage count
				for sf in _filesSource do (--source file
					_pCnt+=1.0;					
					local tf = _scriptDir+"\\"+filenameFromPath sf;--target file					
					local accessFile = deleteFile tf;--try to delete the icons if they can not be replaced below
					local accessFile = copyFile sf tf;
				)
			)
			else if ((maxVersion())[1] / 1000)==14 then(
				local _scriptDir = pathConfig.GetDir #maxroot + "\\plugins";				
				local _filesSource = getFiles (final_url+"\\maxawdexporter_2012_"+getBitVersionAsString()+".dle");	
				for sf in _filesSource do (--source file
					local tf = _scriptDir+"\\"+filenameFromPath sf;--target file					
					local accessFile = deleteFile tf;--try to delete the icons if they can not be replaced below
					local accessFile = copyFile sf tf;
				)
				local _scriptDir = pathConfig.GetDir #ui;				
				local _filesSource = getFiles (final_url+"\\AWDToolBar_2011_2012.CUI");
				local _pCnt = 0.0;--percentage count
				for sf in _filesSource do (--source file
					_pCnt+=1.0;					
					local tf = _scriptDir+"\\"+filenameFromPath sf;--target file					
					local accessFile = deleteFile tf;--try to delete the icons if they can not be replaced below
					local accessFile = copyFile sf tf;
				)
			)
			else if ((maxVersion())[1] / 1000)==15 then(
				local _scriptDir = pathConfig.GetDir #maxroot + "\\plugins";				
				local _filesSource = getFiles (final_url+"\\maxawdexporter_2013_"+getBitVersionAsString()+".dle");
				for sf in _filesSource do (--source file
					local tf = _scriptDir+"\\"+filenameFromPath sf;--target file					
					local accessFile = deleteFile tf;--try to delete the icons if they can not be replaced below
					local accessFile = copyFile sf tf;
				)
				local _scriptDir = pathConfig.GetDir #ui;				
				local _filesSource = getFiles (final_url+"\\AWDToolBar_2013_2014.cuix");
				for sf in _filesSource do (--source file
					local tf = _scriptDir+"\\"+filenameFromPath sf;--target file					
					local accessFile = deleteFile tf;--try to delete the icons if they can not be replaced below
					local accessFile = copyFile sf tf;
				)
			)
			else if ((maxVersion())[1] / 1000)==16 then(
				local _scriptDir = pathConfig.GetDir #maxroot + "\\plugins"		
				local _filesSource = getFiles (final_url+"\\maxawdexporter_2014_"+getBitVersionAsString()+".dle")
				local _pCnt = 0.0;--percentage count
				for sf in _filesSource do (--source file
					_pCnt+=1.0;					
					local tf = _scriptDir+"\\"+filenameFromPath sf;--target file					
					local accessFile = deleteFile tf;--try to delete the icons if they can not be replaced below
					local accessFile = copyFile sf tf;
				)
				local _scriptDir = pathConfig.GetDir #ui;				
				local _filesSource = getFiles (final_url+"\\AWDToolBar_2013_2014.cuix")
				local _pCnt = 0.0;--percentage count
				for sf in _filesSource do (--source file
					_pCnt+=1.0;					
					local tf = _scriptDir+"\\"+filenameFromPath sf;--target file					
					local accessFile = deleteFile tf--try to delete the icons if they can not be replaced below
					local accessFile = copyFile sf tf
				)
			)
			else (
				messageBox ("Sorry!\n\nAt this point AwayExtensions-3dsmax only supports 3dsmax 2011 / 2012 / 2013 / 2014")
			)
			
			loadDllsFromDir (pathConfig.GetDir #maxroot + "\\plugins\\") "maxawdexporter*"
			
			setup_AWDExtensions_update 25 "Create '/plugins/AwayExtensions3dsMax' folder";
			
			local _scriptDir = pathConfig.GetDir #maxroot + "\\plugins\\AwayExtensions3dsMax";
			local _filesSource = getFiles (final_url+"\\AwayExtensions3dsmax\\*");
			--print("tis = "+_filesSource[1]);
			local _pCnt = 0.0;
			for sf in _filesSource do (
				_pCnt+=1.0;				
				local tf = _scriptDir+"\\"+filenameFromPath sf;				
				local accessFile = deleteFile tf;
				local accessFile = copyFile sf tf;				
				if (accessFile == true)then(
					local p = (_pCnt / _filesSource.count) * 50.0+25.0;
					setup_AWDExtensions_update p ("copy to '\plugins\AwayExtensions3dsMax' file "+sf as string);
					print("copy files..."+sf as string);
				)else(
					setup_AWDExtensions_update 0 "Error, can't copy files to the '\plugins\AwayExtensions3dsMax' folder";
					print("fail..");
					failing = true;
				)
			)
			local _scriptDir = pathConfig.GetDir #maxroot + "\\plugins\\AwayExtensions3dsMax\\AWDModifier\\"
			local accessFolder = makeDir _scriptDir;
			local _filesSource = getFiles (final_url+"\\AwayExtensions3dsmax\\AWDModifier\\*")
			for sf in _filesSource do (
				_pCnt+=1.0;				
				local tf = _scriptDir+"\\"+filenameFromPath sf			
				local accessFile = deleteFile tf--try to delete the icons if they can not be replaced below
				local accessFile = copyFile sf tf			
				if (accessFile == true)then(
					local p = (_pCnt / _filesSource.count) * 50.0+25.0
					setup_AWDExtensions_update p ("copy to '\plugins\AwayExtensions3dsMax' file "+sf as string)
					print("copy files..."+sf as string)
				)else(
					setup_AWDExtensions_update 0 "Error, can't copy files to the '\plugins\AwayExtensions3dsMax' folder"
					print("fail..")
					failing = true
				)
			)
			local _scriptDir = pathConfig.GetDir #maxroot + "\\plugins\\AwayExtensions3dsMax\\AWDHTMLViewer\\"
			local accessFolder = makeDir _scriptDir;
			local _filesSource = getFiles (final_url+"\\AwayExtensions3dsmax\\AWDHTMLViewer\\*")
			for sf in _filesSource do (
				_pCnt+=1.0;				
				local tf = _scriptDir+"\\"+filenameFromPath sf			
				local accessFile = deleteFile tf--try to delete the icons if they can not be replaced below
				local accessFile = copyFile sf tf			
				if (accessFile == true)then(
					local p = (_pCnt / _filesSource.count) * 50.0+25.0
					setup_AWDExtensions_update p ("copy to '\plugins\AwayExtensions3dsMax' file "+sf as string)
					print("copy files..."+sf as string)
				)else(
					setup_AWDExtensions_update 0 "Error, can't copy files to the '\plugins\AwayExtensions3dsMax' folder"
					print("fail..")
					failing = true
				)
			)
			local _scriptDir = pathConfig.GetDir #maxroot + "\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings\\";
			local accessFolder = makeDir _scriptDir;
			local _filesSource = getFiles (final_url+"\\AwayExtensions3dsmax\\AWDCustomSettings\\*");
			for sf in _filesSource do (
				_pCnt+=1.0;				
				local tf = _scriptDir+"\\"+filenameFromPath sf;				
				local accessFile = deleteFile tf;--try to delete the icons if they can not be replaced below
				local accessFile = copyFile sf tf;				
				if (accessFile == true)then(
					local p = (_pCnt / _filesSource.count) * 50.0+25.0;
					setup_AWDExtensions_update p ("copy to '\plugins\AwayExtensions3dsMax' file "+sf as string);
					print("copy files..."+sf as string);
				)else(
					setup_AWDExtensions_update 0 "Error, can't copy files to the '\plugins\AwayExtensions3dsMax' folder";
					print("fail..");
					failing = true;
				)
			)
			local _scriptDir = pathConfig.GetDir #userMacros + "\\AWDCommands\\";
			local accessFolder = makeDir _scriptDir;
			local _filesSource = getFiles (final_url+"\\AWDCommands\\*");
			for sf in _filesSource do (
				_pCnt+=1.0;				
				local tf = _scriptDir+"\\"+filenameFromPath sf;			
				print tf				
				local accessFile = deleteFile tf;--try to delete the icons if they can not be replaced below
				local accessFile = copyFile sf tf;				
				if (accessFile == true)then(
					local p = (_pCnt / _filesSource.count) * 50.0+25.0;
					setup_AWDExtensions_update p ("copy to '\userMacros\AWDCommands' file "+sf as string);
					print("copy files..."+sf as string);
				)else(
					setup_AWDExtensions_update 0 "Error, can't copy files to the '\userMacros\AWDCommands' folder";
					print("fail..");
					failing = true;
				)
			)
			
			
			if (failing == false)then(
				macros.load (pathConfig.GetDir #userMacros + "\\AWDCommands\\")
				--local _iconDir = pathConfig.GetDir #ui+"Icons";
				local _iconDir = pathConfig.GetDir #userIcons;
				
				local _filesSource = getFiles (final_url+"Icons\\*");
				
				
				local _pCnt = 0.0;--percentage count
				for sf in _filesSource do (--source file
					_pCnt+=1.0;
					
					local tf = _iconDir+"\\"+filenameFromPath sf;--target file
					
					local accessFile = deleteFile tf;--try to delete the icons if they can not be replaced below
					local accessFile = copyFile sf tf;
					
					if (accessFile == true)then(
						local p = (_pCnt / _filesSource.count) * 20.0+75.0;
						setup_AWDExtensions_update p ("copy to '\ui\Icons' file "+_pCnt as string+"/"+_filesSource.count as string);
					)else(
						setup_AWDExtensions_update 0 "Error, can't copy Icons to the '/userIcons' folder";
						failing = true;
					)
				)
				
				if (failing == false)then(
					
					setup_AWDExtensions_update 95 "reload 3dsmax toolbar Icons (max9+)";
					if ((maxVersion())[1] >= 9000 )then(
						colorman.reInitIcons();--after the icons are copied to the UI folder, reload Max Toolbar icons so that the texToolbar icon is visible (max bug)
					)else(
						messageBox "Because you have an older version of max (below version 9) you need to restart max\nin order to see AWDExtensions icons on the main tollbar, that is should you drag AWDExtensions in the main toolbar." title:"Restart Max" beep:false;
					)
					
					setup_AWDExtensions_update 98 "starting AWDExtensions...";
					
					--local startFile = _scriptDir+"\\AWDExtensions_3.00.ms"
					--fileIn startFile;--start AWDExtensions
					
					setup_AWDExtensions_update 100 "installation was succesfull!";
					
					CreateDialog rl_setup 280 342;
					
					try (--close if possible
						DestroyDialog rl_install;--this is the last step and it will delete the left over MZP files
					)catch();
				)
			)
			
		)else(
			setup_AWDExtensions_update 0 "Error, no writing rights within your '/userscripts' folder.\nPoked around with some settings in 3dsMax?";
			setup_AWDExtensions_install_manually();
		)
	)
	else(
		setup_AWDExtensions_update 0 "Error, no temporary MZP files found:\nRedrag the MZP file into the viewport and install again.";
		setup_AWDExtensions_install_manually();
	)
)


function setup_AWDExtensions_uninstall_getList =(
	
	--generate a list of files to delete
	local del_folders =#();
	local del_files =#();
	
	local mzpFolder = setup_AWDExtensions_find_MZP_url();--get the current MZP folder
	
	--clearlistener();
	local folders = getDirectories (sysInfo.tempdir+"mzptmp*");--temporay MZP folders
	for f in folders do (
		local  has_folder= getDirectories (f+"AwayExtensions3dsMax");
		local  has_mzp= getFiles (f+"mzp.run");
		if (has_folder.count == 1 and has_mzp.count == 1)then(--mark this folder to be deleted
			
			local dirA = getFilenamePath mzpFolder;
			local dirB = getFilenamePath f;
			
			
			if (dirA == dirB)then(
				print("EXCLUDE ");
			)else(
				print("NOT");
				append del_folders f;
			
				local files = getFiles (f+"\\*");--delete files inside this folder before we can delete the folder
				for fl in files do (
					append del_files fl;
				)
				local files = getDirectories (f+"\\*");--delete files inside this folder before we can delete the folder
				for fldr in files do (
					local files2 = getFiles (fldr+"\\*");--delete files inside this folder before we can delete the folder
					for fl in files2 do (
						append del_files fl;
					)
				)
				
				
			)
		)
	)
	--fheck for the scripts/AWDExtensions folder
	local  has_folder= getDirectories ((pathConfig.GetDir #maxroot)+"\\plugins\\AwayExtensions3dsMax");
	if (has_folder.count == 1)then(--mark this folder to be deleted
		local  has_folder2= getDirectories ((pathConfig.GetDir #maxroot)+"\\plugins\\AwayExtensions3dsMax\\AWDCustomSettings");
		if (has_folder2.count == 1)then(--mark this folder to be deleted
			append del_folders has_folder2[1];
			local files = getFiles (has_folder2[1]+"\\*");--delete files inside this folder before we can delete the folder
			for f in files do (
				append del_files f;
			)
		)
		local  has_folder2= getDirectories ((pathConfig.GetDir #maxroot)+"\\plugins\\AwayExtensions3dsMax\\AWDModifier");
		if (has_folder2.count == 1)then(--mark this folder to be deleted
			append del_folders has_folder2[1];
			local files = getFiles (has_folder2[1]+"\\*");--delete files inside this folder before we can delete the folder
			for f in files do (
				append del_files f;
			)
		)
		local  has_folder2= getDirectories ((pathConfig.GetDir #maxroot)+"\\plugins\\AwayExtensions3dsMax\\AWDHTMLViewer");
		if (has_folder2.count == 1)then(--mark this folder to be deleted
			append del_folders has_folder2[1];
			local files = getFiles (has_folder2[1]+"\\*");--delete files inside this folder before we can delete the folder
			for f in files do (
				append del_files f;
			)
		)
		append del_folders has_folder[1];
		local files = getFiles (has_folder[1]+"\\*");--delete files inside this folder before we can delete the folder
		for f in files do (
			append del_files f;
		)
	)
	
	
	local  has_folder= getDirectories ((pathConfig.GetDir #userMacros)+"\\AWDCommands");--temporay MZP folders
	if (has_folder.count == 1)then(--mark this folder to be deleted
		append del_folders has_folder[1];
		local files = getFiles (has_folder[1]+"\\*");--delete files inside this folder before we can delete the folder
		for f in files do (
			append del_files f;
		)
	)
	local files = getFiles ((pathConfig.GetDir #userMacros)+"\\AWDCommands*");--temporay MZP folders
	for f in files do (
		append del_files f;
	)
	local files = getFiles ((pathConfig.GetDir #ui)+"\\AWDToolBar*");
	for f in files do (
		append del_files f;
	)
	--icon files
	local files = getFiles ((pathConfig.GetDir #ui+"Icons")+"\\AwayExtensionIcons*");
	for f in files do (
		append del_files f;
	)
	local files = getFiles ((pathConfig.GetDir #userIcons)+"\\AwayExtensionIcons*");
	for f in files do (
		append del_files f;
	)
	return #(del_files, del_folders)
)


function setup_AWDExtensions_uninstall_step2=(
	
	local list = setup_AWDExtensions_uninstall_getList();
	print("Uninstall");
	
	for s in list[1] do (
		--files
		local _done = deleteFile s;
		--format ("\t"+s+", done: "+_done as string+"\n");
	)
	for s in list[2] do (
		local str = "rmdir \""+s+"\"";
		local _done;
		hiddendoscommand str startpath:"$max" exitCode:&_done
		--format (str+"\t\tdone: "+_done as string+"\n");
	)
	
	if (list[1].count == 0 and list[2].count == 0)then(
		--go back
		
		if ((maxVersion())[1] / 1000)==13 then(
			messageBox("Cannot delete the plugin-file '"+#maxroot + "\plugins\maxawdexporter_2011_"+getBitVersionAsString()+".dle', because it is currently loaded.\n Please close 3dsmax and delete the plugin manually.")
		)
		else if ((maxVersion())[1] / 1000)==14 then(
			messageBox("Cannot delete the plugin-file '"+#maxroot + "\plugins\maxawdexporter_2012_"+getBitVersionAsString()+".dle', because it is currently loaded.\n Please close 3dsmax and delete the plugin manually.")
		)
		else if ((maxVersion())[1] / 1000)==15 then(
			messageBox("Cannot delete the plugin-file '"+#maxroot + "\plugins\maxawdexporter_2013_"+getBitVersionAsString()+".dle', because it is currently loaded.\n Please close 3dsmax and delete the plugin manually.")
		)
		else if ((maxVersion())[1] / 1000)==16 then(
			messageBox("Cannot delete the plugin-file '"+#maxroot + "\plugins\maxawdexporter_2014_"+getBitVersionAsString()+".dle', because it is currently loaded.\n Please close 3dsmax and delete the plugin manually.")
		)
		try (--close if possible
			DestroyDialog rl_uninstall;
		) catch();
		try (--close if possible
			DestroyDialog rl_install;
		) catch();
		
	)
	else(
		setup_AWDExtensions_uninstall_step1();
	)
	
	print("Done "+setup_AWDExtensions_uninstall_step1 as string);
)


function setup_AWDExtensions_uninstall_step1=(
	local list = setup_AWDExtensions_uninstall_getList();
	print("........list ;."+list[1].count as string);
	
	--create dropdown list array
	local list2 = #();
	for f in list[2] do(
		append list2 ("folder: "+ f)
	)
	for f in list[1] do(
		append list2 ("file:   "+filenameFromPath  f)
	)
	
	--print(" GUI "+rl_uninstall as string);
	
	rl_uninstall.lbx_list.items = list2;
	if (list2.count == 0)then(
		if ((maxVersion())[1] / 1000)==13 then(
			messageBox("Cannot delete the plugin-file '"+#maxroot + "\plugins\maxawdexporter_2011_"+getBitVersionAsString()+".dle', because it is currently loaded.\n Please close 3dsmax and delete the plugin manually.")
		)
		else if ((maxVersion())[1] / 1000)==14 then(
			messageBox("Cannot delete the plugin-file '"+#maxroot + "\plugins\maxawdexporter_2012_"+getBitVersionAsString()+".dle', because it is currently loaded.\n Please close 3dsmax and delete the plugin manually.")
		)
		else if ((maxVersion())[1] / 1000)==15 then(
			messageBox("Cannot delete the plugin-file '"+#maxroot + "\plugins\maxawdexporter_2013_"+getBitVersionAsString()+".dle', because it is currently loaded.\n Please close 3dsmax and delete the plugin manually.")
		)
		else if ((maxVersion())[1] / 1000)==16 then(
			messageBox("Cannot delete the plugin-file '"+#maxroot + "\plugins\maxawdexporter_2014_"+getBitVersionAsString()+".dle', because it is currently loaded.\n Please close 3dsmax and delete the plugin manually.")
		)
		rl_uninstall.lbl_message.text = ("No AwayExtensions3dsMax files left on your System:");
		rl_uninstall.btn_uninstall.caption = "Back";
		try (--close if possible
			DestroyDialog rl_uninstall;
		) catch();
		try (--close if possible
			DestroyDialog rl_install;
		) catch();
	)else(
		rl_uninstall.lbl_message.text = ("Are yout sure to delete these "+list[2].count as string+" folder(s) and "+list[1].count as string+" file(s)?");
	)
)


---------------------
rollout rl_setup "Installation complete" width:280 height:346
(
	button btn_close "Close" pos:[179,310] width:100 height:32
	bitmap bmp1 "Bitmap" pos:[0,0] width:280 height:310 fileName:"setup.bmp"
	label lbl2 "The installation was completed succesfully. Have Fun!" pos:[3,312] width:176 height:30
	on btn_close pressed do
	(
		try (--close if possible
			DestroyDialog rl_setup;
		) catch();
	)
)
---------------------
rollout rl_uninstall "Uninstall" width:280 height:312
(
	button btn_uninstall "Uninstall" pos:[4,246] width:274 height:32
	progressBar pb_bar "ProgressBar" pos:[5,280] width:274 height:15 color:[190,10,30]
	label txt_stats "" pos:[7,297] width:270 height:14
	MultiListBox  lbx_list "" pos:[2,18] width:280 height:17 readOnly:true;
	label lbl_message "..." pos:[3,3] width:280 height:14
	on btn_uninstall pressed do
	(
		setup_AWDExtensions_uninstall_step2();
	)
	on rl_uninstall close do(
		print("You closed the uninstall...");
		try (--close if possible
			DestroyDialog rl_uninstall;
		) catch();
		CreateDialog rl_install 280 100;
	)
)
---------------------
rollout rl_install "" width:280 height:100
(
	button btn_install "Install" pos:[0,32] width:140 height:32
	button btn_uninstall "Uninstall" pos:[140,32] width:140 height:32
	progressBar pb_bar "ProgressBar" pos:[3,67] width:274 height:15 color:[190,10,30]
	
	bitmap bmp1 "Bitmap" pos:[0,0] width:280 height:32 fileName:"install.bmp"
	
	label txt_stats "Click 'Install' to install AwayExtensions3dsmax." pos:[4,86] width:270 height:60

	
	on rl_install close do
	(
			local visUninstall = rl_uninstall.open;
			
			if (visUninstall == false)then(--only delete the MZP files if no other essential setup floaters are visible
				setup_AWDExtensions_quit();--delete any MZP temporay left files
				gc();
			)
		)
	on btn_install pressed do
	(
			setup_AWDExtensions_install();
		)
	on btn_uninstall pressed do
	(
		CreateDialog rl_uninstall 280 316;
		setup_AWDExtensions_uninstall_step1();
		try (--close if possible
			DestroyDialog rl_install;
			rl_install
		) catch();
	)
)



CreateDialog rl_install 280 100;
rl_install.title = "AWDExtensions version "+AWDExtensions_version_nr+" Setup";
if ((maxVersion())[1] / 1000)==13 then()
else if ((maxVersion())[1] / 1000)==14 then()
else if ((maxVersion())[1] / 1000)==15 then()
else if ((maxVersion())[1] / 1000)==16 then()
else (
	try (--close if possible
		DestroyDialog rl_install;
		rl_install
	) catch();
	messageBox ("Sorry!\n\nAt this point AwayExtensions-3dsmax only supports 3dsmax 2011 / 2012 / 2013 / 2014")
)