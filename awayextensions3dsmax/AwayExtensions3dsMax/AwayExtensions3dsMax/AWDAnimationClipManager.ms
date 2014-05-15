fn CreateNativeWindowOps forceRecompile:on = if forceRecompile do
(
	source = ""
	source += "using System;\n"
	source += "using System.Windows.Forms;\n"
	source += "public class WindowHook : NativeWindow\n"
	source += "{\n"
	source += " private const int WM_GETDLGCODE = 0x0087;\n"
	source += " private const int DLGC_WANTALLKEYS = 0x0004;\n"
	source += " public WindowHook() { }\n"
	source += " protected override void WndProc(ref Message m)\n"
	source += " {\n"
	source += " switch (m.Msg)\n"
	source += " {\n"
	source += " case WM_GETDLGCODE:\n"
	source += " m.Result = (IntPtr)DLGC_WANTALLKEYS;\n"
	source += " break;\n"
	source += " default:\n"
	source += " base.WndProc(ref m);\n"
	source += " break;\n"
	source += " }\n"
	source += " }\n"
	source += "}\n"

	csharpProvider = dotnetobject "Microsoft.CSharp.CSharpCodeProvider"
	compilerParams = dotnetobject "System.CodeDom.Compiler.CompilerParameters"

	compilerParams.ReferencedAssemblies.AddRange #("System.dll", "System.Windows.Forms.dll")

	compilerParams.GenerateInMemory = true
	compilerResults = csharpProvider.CompileAssemblyFromSource compilerParams #(source)

	global NativeWindowOps = compilerResults.CompiledAssembly
)
CreateNativeWindowOps()

fn initSource  thisRollout = (
	global thisMode
	global thisSourceObj
	
	rowCnt=thisRollout.myDGV.rows.count-1
	for i=0 to rowCnt do thisRollout.myDGV.Rows.Remove(thisRollout.myDGV.rows.item[0])		
	newNames=#()
	thisClassName=(classof thisSourceObj as string)
	if thisClassName=="AWDSkeleton" or thisClassName=="AWDSkeletonClone" then(
		thisMode="Skeleton"
	)
	else if thisClassName=="AWDVertexAnimSource" or thisClassName=="AWDVertexAnimSourceClone" then(
		thisMode="Vertex"
	)
	else(
		thisMode="UV"
	)

	if thisMode=="Vertex" or thisMode=="Skeleton" then(
		thisRollout.lbl_selectedClone.enabled=true
		thisRollout.btn_select_in_modPanel.enabled=true
		thisRollout.selectedClone.enabled=true
		thisRollout.selectedClone.items=#("original")
		availableClones=#(thisSourceObj)
		for i=1 to thisSourceObj.savedClones.count do
		(
			append availableClones thisSourceObj.savedClones[i]
			thisRollout.selectedClone.items= append thisRollout.selectedClone.items thisSourceObj.savedClones[i].name
		)
		thisRollout.selectedClone.selection=1
	)
	else if thisMode=="UV" then (
		thisRollout.lbl_selectedClone.enabled=false
		thisRollout.selectedClone.enabled=true
		thisRollout.btn_select_in_modPanel.enabled=false
		thisRollout.selectedClone.items=#(thisSourceObj.name)
		availableClones=#(thisSourceObj)
		thisRollout.selectedClone.selection=1
		thisRollout.selectedClone.enabled=false
	)
	validateClips thisSourceObj
	for i=1 to thisSourceObj.saveAnimClipNames.count do
	(
		thisRollout.myDGV.rows.add ()
		try thisRollout.myDGV.rows.item [i-1].cells.item [0].value = thisSourceObj.saveAnimClipNames [i] catch thisRollout.myDGV.rows.item [i-1].cells.item [0].value=""
		try thisRollout.myDGV.rows.item [i-1].cells.item [1].value = thisSourceObj.saveStartFrames[i] catch thisRollout.myDGV.rows.item [i-1].cells.item [1].value=1
		try thisRollout.myDGV.rows.item [i-1].cells.item [2].value = thisSourceObj.saveEndFrames[i] catch thisRollout.myDGV.rows.item [i-1].cells.item [2].value=2
		try thisRollout.myDGV.rows.item [i-1].cells.item [3].value = thisSourceObj.saveSkipFrames[i] catch thisRollout.myDGV.rows.item [i-1].cells.item [3].value=0
		try thisRollout.myDGV.rows.item [i-1].cells.item [6].value = thisSourceObj.saveStitchFinals[i] catch thisRollout.myDGV.rows.item [i-1].cells.item [6].value=false
		try thisRollout.myDGV.rows.item [i-1].cells.item [7].value = thisSourceObj.saveLoops[i] catch thisRollout.myDGV.rows.item [i-1].cells.item [7].value=false
		try thisRollout.myDGV.rows.item [i-1].cells.item [8].value = thisSourceObj.saveTransform[i] catch thisRollout.myDGV.rows.item [i-1].cells.item [8].value=false
		editCell 100 (i-1) thisrollout					
	)
)
	
fn initClonedSource thisrollout = (
	global thisSourceObj
	rowCnt=thisrollout.myDGV.rows.count-1
	for i=0 to rowCnt do thisrollout.myDGV.Rows.Remove(thisrollout.myDGV.rows.item[0])		
	for i=1 to thisSourceObj.saveAnimClipNames.count do
	(
		thisrollout.myDGV.rows.add ()
		try thisrollout.myDGV.rows.item [i-1].cells.item [0].value = thisSourceObj.saveAnimClipNames [i] catch thisrollout.myDGV.rows.item [i-1].cells.item [0].value=""
		try thisrollout.myDGV.rows.item [i-1].cells.item [1].value = thisSourceObj.saveStartFrames[i] catch thisrollout.myDGV.rows.item [i-1].cells.item [1].value=1
		try thisrollout.myDGV.rows.item [i-1].cells.item [2].value = thisSourceObj.saveEndFrames[i] catch thisrollout.myDGV.rows.item [i-1].cells.item [2].value=2
		try thisrollout.myDGV.rows.item [i-1].cells.item [3].value = thisSourceObj.saveSkipFrames[i] catch thisrollout.myDGV.rows.item [i-1].cells.item [3].value=0
		try thisrollout.myDGV.rows.item [i-1].cells.item [6].value = thisSourceObj.saveStitchFinals[i] catch thisrollout.myDGV.rows.item [i-1].cells.item [6].value=false
		try thisrollout.myDGV.rows.item [i-1].cells.item [7].value = thisSourceObj.saveLoops[i] catch thisrollout.myDGV.rows.item [i-1].cells.item [7].value=false
		try thisrollout.myDGV.rows.item [i-1].cells.item [8].value = thisSourceObj.saveTransform[i] catch thisrollout.myDGV.rows.item [i-1].cells.item [8].value=false
		editCell 100 (i-1) thisrollout
	)
)
		
fn updateRowFromUI rowIndx thisrollout= (
	global thisSourceObj
	global thisMode
	currentSourceActive=thisSourceObj
	rowCnt=thisrollout.myDGV.rows.count
	if (rowIndx<thisrollout.myDGV.rows.count) then (
		--if currentSourceActive==undefined then return()
		--print currentSourceActive.name
		
		currentSourceActive.saveAnimClipNames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [0].value
		--if coloumIdx==2 then currentSourceActive.saveAnimClipNames[rowIndx+1]=myDGV.rows.item [rowIndx].cells.item [2].value
			oldValue=currentSourceActive.saveStartFrames[rowIndx+1]
			try currentSourceActive.saveStartFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [1].value as integer catch(
				currentSourceActive.saveStartFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [1].value=oldValue
			)
			if currentSourceActive.saveStartFrames[rowIndx+1]>=currentSourceActive.saveEndFrames[rowIndx+1] then (
				currentSourceActive.saveStartFrames[rowIndx+1]=currentSourceActive.saveEndFrames[rowIndx+1]-1
				thisrollout.myDGV.rows.item [rowIndx].cells.item [1].value=currentSourceActive.saveStartFrames[rowIndx+1]
			)		
			oldValue=currentSourceActive.saveEndFrames[rowIndx+1]
			try currentSourceActive.saveEndFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [2].value as integer catch(
				currentSourceActive.saveEndFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [2].value=oldValue
			)
			if currentSourceActive.saveEndFrames[rowIndx+1]<=currentSourceActive.saveStartFrames[rowIndx+1] then (
				currentSourceActive.saveEndFrames[rowIndx+1]=currentSourceActive.saveStartFrames[rowIndx+1]+1
				thisrollout.myDGV.rows.item [rowIndx].cells.item [2].value=currentSourceActive.saveEndFrames[rowIndx+1]
			)
			oldValue=currentSourceActive.saveSkipFrames[rowIndx+1]
			try currentSourceActive.saveSkipFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [3].value as float catch(
				currentSourceActive.saveSkipFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [3].value=oldValue
			)
		currentSourceActive.saveStitchFinals[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [6].value
		currentSourceActive.saveLoops[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [7].value
		currentSourceActive.saveTransform[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [8].value
			print (thisrollout.myDGV.rows.item [rowIndx].cells.item [8].value as string)
		try(
			if currentSourceActive.saveSkipFrames[rowIndx+1]>=((currentSourceActive.saveEndFrames[rowIndx+1]-currentSourceActive.saveStartFrames[rowIndx+1])-1) then (
				currentSourceActive.saveSkipFrames[rowIndx+1]=((currentSourceActive.saveEndFrames[rowIndx+1]-currentSourceActive.saveStartFrames[rowIndx+1])-1)
				thisrollout.myDGV.rows.item [rowIndx].cells.item [3].value=currentSourceActive.saveSkipFrames[rowIndx+1]
			)
			startFrame=currentSourceActive.saveStartFrames[rowIndx+1]
			endFrame=currentSourceActive.saveEndFrames[rowIndx+1]
			allFrame=(endFrame-startFrame)
			thisrollout.myDGV.rows.item[rowIndx].Cells.Item[4].value= ((allFrame + 1 )*((1.0/frameRate) as float)*1000) as string
			if (currentSourceActive.saveSkipFrames[rowIndx+1])>0 then (
				intKeys=((allFrame-1)/(currentSourceActive.saveSkipFrames[rowIndx+1]+1)as integer) 
				numberkeys=((allFrame-1)/(currentSourceActive.saveSkipFrames[rowIndx+1]+1)as float)
				if(intKeys<numberkeys) then intKeys+=1
				thisrollout.myDGV.rows.item[rowIndx].Cells.Item[5].value	= ((2+intKeys) as string)
			)
			else(
				thisrollout.myDGV.rows.item[rowIndx].Cells.Item[5].value	= ((allFrame+1) as string)
			)
		)
		catch(print "error whe calculating keyframe count")
		updateUI currentSourceActive rowIndx
	)
)	
fn editCell coloumIdx rowIndx thisrollout = (
	global thisSourceObj
	global thisMode
	currentSourceActive=thisSourceObj
	--if currentSourceActive==undefined then return()
	--print currentSourceActive.name
	if coloumIdx==0 then currentSourceActive.saveAnimClipNames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [0].value
	--if coloumIdx==2 then currentSourceActive.saveAnimClipNames[rowIndx+1]=myDGV.rows.item [rowIndx].cells.item [2].value
	if coloumIdx==1 then (
		oldValue=currentSourceActive.saveStartFrames[rowIndx+1]
		try currentSourceActive.saveStartFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [1].value as integer catch(
			currentSourceActive.saveStartFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [1].value=oldValue
		)
		if currentSourceActive.saveStartFrames[rowIndx+1]>=currentSourceActive.saveEndFrames[rowIndx+1] then (
			currentSourceActive.saveStartFrames[rowIndx+1]=currentSourceActive.saveEndFrames[rowIndx+1]-1
			thisrollout.myDGV.rows.item [rowIndx].cells.item [1].value=currentSourceActive.saveStartFrames[rowIndx+1]
		)			
	)
	if coloumIdx==2 then (
		oldValue=currentSourceActive.saveEndFrames[rowIndx+1]
		try currentSourceActive.saveEndFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [2].value as integer catch(
			currentSourceActive.saveEndFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [2].value=oldValue
		)
		if currentSourceActive.saveEndFrames[rowIndx+1]<=currentSourceActive.saveStartFrames[rowIndx+1] then (
			currentSourceActive.saveEndFrames[rowIndx+1]=currentSourceActive.saveStartFrames[rowIndx+1]+1
			thisrollout.myDGV.rows.item [rowIndx].cells.item [2].value=currentSourceActive.saveEndFrames[rowIndx+1]
		)
	)
	if coloumIdx==3 then (
		oldValue=currentSourceActive.saveSkipFrames[rowIndx+1]
		try currentSourceActive.saveSkipFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [3].value as float catch(
			currentSourceActive.saveSkipFrames[rowIndx+1]=thisrollout.myDGV.rows.item [rowIndx].cells.item [3].value=oldValue
		)
	)
	if coloumIdx==6 then currentSourceActive.saveStitchFinals[rowIndx+1] = thisrollout.myDGV.rows.item [rowIndx].cells.item [6].value
	if coloumIdx==7 then currentSourceActive.saveLoops[rowIndx+1] = thisrollout.myDGV.rows.item [rowIndx].cells.item [7].value
	if coloumIdx==8 then currentSourceActive.saveTransform[rowIndx+1] = thisrollout.myDGV.rows.item [rowIndx].cells.item [8].value
	try(
		if currentSourceActive.saveSkipFrames[rowIndx+1]>=((currentSourceActive.saveEndFrames[rowIndx+1]-currentSourceActive.saveStartFrames[rowIndx+1])-1) then (
			currentSourceActive.saveSkipFrames[rowIndx+1]=((currentSourceActive.saveEndFrames[rowIndx+1]-currentSourceActive.saveStartFrames[rowIndx+1])-1)
			thisrollout.myDGV.rows.item [rowIndx].cells.item [3].value=currentSourceActive.saveSkipFrames[rowIndx+1]
		)
		startFrame=currentSourceActive.saveStartFrames[rowIndx+1]
		endFrame=currentSourceActive.saveEndFrames[rowIndx+1]
		allFrame=(endFrame-startFrame)
		thisrollout.myDGV.rows.item[rowIndx].Cells.Item[4].value= ((allFrame + 1 )*((1.0/frameRate) as float)*1000) as string
		if (currentSourceActive.saveSkipFrames[rowIndx+1])>0 then (
			intKeys=((allFrame-1)/(currentSourceActive.saveSkipFrames[rowIndx+1]+1)as integer) 
			numberkeys=((allFrame-1)/(currentSourceActive.saveSkipFrames[rowIndx+1]+1)as float)
			if(intKeys<numberkeys) then intKeys+=1
			thisrollout.myDGV.rows.item[rowIndx].Cells.Item[5].value	= ((2+intKeys) as string)
		)
		else(
			thisrollout.myDGV.rows.item[rowIndx].Cells.Item[5].value	= ((allFrame+1) as string)
		)
	)
	catch(print "error whe calculating keyframe count")
	updateUI currentSourceActive rowIndx
)	

fn openAWDClipManager thisAWDAnimSource animType animNrInput= (
	global awd_animation_rollout
	fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDAnimHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
	
	hasDialog=true
	dialogPosition=[0,0];
	dialogWidth=800;
	dialogHeight=400;
	try (
		dialogPosition=(GetDialogPos awd_animation_rollout)
		dialogWidth=awd_animation_rollout.width;
		dialogHeight=awd_animation_rollout.height;
		destroyDialog awd_animation_rollout
		) 
	catch (
		hasDialog=false
	)
	CreateNativeWindowOps()
	--clearListener ()
	global thisMode=animType
	global thisSourceObj=thisAWDAnimSource
	global animNr=animNrInput-1
	validateClips thisSourceObj
	global availableSources=#()
	global availableClones=#()
	global clipBoard=#()
	rollout awd_animation_rollout "AWD Animation Clip Manager" width:840 height:420	(
			button btn_ReInit "Refresh" pos:[10,10] height:30 width:60 enabled:true
			label lbl_selectedSource "AnimationSource:" pos:[80,18]	
			dropDownList selectedSource "" items:#("sdas", "adas") width:180 pos:[170,13]	
			label lbl_selectedClone "Clone:" pos:[400,18]	
			dropDownList selectedClone "" items:#("sdas", "adas") width:100  pos:[435,13]	
			button btn_select_in_modPanel "Select in Mod-Panel" pos:[670,11] height:30 width:120			
			--label lbl_currentMode "" pos:[10,45] width:500

			local hook = dotnetobject "WindowHook"

			dotNetControl myDGV "System.Windows.Forms.DataGridView"	pos:[10,82] 
			button btn_addAnimClip "Add Clip" pos:[10,380] height:30 width:70
			--button btn_preventOverlapping "Prevent overlapping clips" pos:[100,380] height:30 enabled:false
			button btn_delete "Delete" pos:[85,380] height:30 width:70
			button btn_SetRangetoClip "Set Range To Clip" pos:[175,380] height:30 width:120 enabled:false
			button btn_SetRange "Get Range From Clip" pos:[300,380] height:30 width:120 enabled:false
			--button btn_AWDManager "AWD AnimationSet Manager" pos:[440,380] height:30 width:150 enabled:true
			
-- 			on btn_AWDManager pressed do (
-- 				fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDAnimatorManager.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
-- 				openAWDAnimatorManager thisSourceObj "Animator" thisMode 1
-- 				
-- 			)
			on btn_select_in_modPanel pressed do (
				max modify mode 
				modPanel.SetCurrentObject thisSourceObj
			)
			on btn_ReInit pressed do (
				openAWDClipManager thisSourceObj thisMode (animNr+1)
				
			)
			local dragStart=false
			local lastDragY=0
			local	selectedCells=#()
			local myCol1
				
				on btn_SetRange pressed do (
					firstframe=0
					lastframe=0
					valid=false
					i=myDGV.CurrentRow.Index+1
					try(
						firstframe=thisSourceObj.saveStartFrames[i]
						lastframe=thisSourceObj.saveEndFrames[i]
						valid=true
					)
					catch()
					if firstframe<lastframe and valid then animationRange = interval firstframe lastframe --Set range to frames 10 to 50
					else messageBox("Invalid time-range set for the selected clip")
					updateUI thisSourceObj (i-1)
				)
				on btn_SetRangetoClip pressed do (
					i=myDGV.CurrentRow.Index+1
					valid=false
					try(
						thisSourceObj.saveStartFrames[i]=animationRange.start
						thisSourceObj.saveEndFrames[i]=animationRange.end
						try myDGV.rows.item [i-1].cells.item [1].value = thisSourceObj.saveStartFrames[i] catch myDGV.rows.item [i-1].cells.item [1].value=1
						try myDGV.rows.item [i-1].cells.item [2].value = thisSourceObj.saveEndFrames[i] catch myDGV.rows.item [i-1].cells.item [2].value=2
						valid=true
					)
					catch()
					if not valid then messageBox("Error - time range was not set for the clip")
					mtl=undefined
					updateUI thisSourceObj (i-1)
				)
				
			fn resize newSize = (
				if newSize.x<600 then awd_animation_rollout.width=600
				if newSize.y<250 then awd_animation_rollout.height=250
				if newSize.x<600 then newSize.x=600
				if newSize.y<250 then newSize.y=250
					
				myDGV.width=newSize.x-20
				myCol1.width=newSize.x-486
				myDGV.pos.y=50
				myDGV.height=newSize.y-myDGV.pos.y-50
				myDGV.height=newSize.y-myDGV.pos.y-50
				myDGV.height=newSize.y-myDGV.pos.y-50
				btn_select_in_modPanel.pos.x=newSize.x-130
				onewidthHeader=(newSize.x-360)/3
				selectedSource.width=onewidthHeader*2
				lbl_selectedClone.pos.x=onewidthHeader*2 + 180
				selectedClone.pos.x=onewidthHeader*2 + 220
				selectedClone.width=onewidthHeader
				
				--btn_AWDManager.pos.y=newSize.y-40
				btn_addAnimClip.pos.y=newSize.y-40
				btn_delete.pos.y=newSize.y-40
				btn_SetRangetoClip.pos.y=newSize.y-40
				btn_SetRange.pos.y=newSize.y-40
				
				
				
			)
			on awd_animation_rollout resized  newSize do (	
				resize newSize
			)	
			fn onKeyPress s e =
			(
				--format "key press: %\n" (bit.charasint e.keyChar)
			)
			fn onKeyDown s e =
			(
				format "key down: %\n" (e.keyCode.ToString())
			)
			fn onMouseDown s e =
			(
				print "mouse down"
			)
			on myDGV EditingControlShowing s e do -- When the cell is being edited
			(
				hook.AssignHandle (dotnetobject "IntPtr" e.Control.Handle)
				dotNet.removeAllEventHandlers e.Control
				dotNet.addEventHandler e.Control "KeyPress" onKeyPress
				dotNet.addEventHandler e.Control "KeyDown" onKeyDown
			)		
			on myDGV CellEndEdit s e do
			(
				myDGV.commitEdit (dotNetClass "DataGridViewDataErrorContexts").commit
				--format "cell end edit... : %\n" s.rows.item[e.RowIndex].cells.item[e.ColumnIndex].value
				editCell e.ColumnIndex e.RowIndex awd_animation_rollout
				hook.ReleaseHandle()
			)		
			on selectedClone selected thisValue do(
				global thisSourceObj=availableClones[thisValue]
				initClonedSource awd_animation_rollout
			)
			on selectedSource selected thisValue do(
				global thisSourceObj=availableSources[thisValue]
				initSource awd_animation_rollout
			)
			on myDGV SelectionChanged e do ( 
				btn_SetRangetoClip.enabled=false
				btn_SetRange.enabled=false
				btn_delete.enabled=false
				if (myDGV.CurrentRow!=undefined) then (	
					btn_SetRangetoClip.enabled=true
					btn_SetRange.enabled=true
					btn_delete.enabled=true				
					editCell 100 myDGV.CurrentRow.Index awd_animation_rollout
				)
			)
			on btn_preventOverlapping pressed do (
				messageBox ("This function is not implemented yet. It will check if any clips overlap, and offer to fix automaticly.")
			)
			on btn_addAnimClip pressed do (
				irowCnt=myDGV.rows.count+1
				theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDAnimationClip #"+irowCnt as string)
				theobj.text ="Add name for new AWDAnimationClip"
				DialogResult = theObj.Showmodal()
				isOK=dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
				if isOK then(
					result = theobj.InstanceName
					myDGV.rows.add ()
					i=myDGV.rows.count
					append thisSourceObj.saveAnimClipNames result	
					fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
					append thisSourceObj.saveAnimSourceIDs (getAWDID() as string)
					try thisVal=(myDGV.rows.item [i-2].cells.item [1].value+100) catch thisVal=0		
					append thisSourceObj.saveStartFrames thisVal
					try thisVal=(myDGV.rows.item [i-2].cells.item [2].value+100) catch thisVal=100
					append thisSourceObj.saveEndFrames thisVal
					append thisSourceObj.saveSkipFrames 0
					append thisSourceObj.saveStitchFinals false
					append thisSourceObj.saveLoops false
					append thisSourceObj.saveTransform false
					try myDGV.rows.item [i-1].cells.item [0].value = thisSourceObj.saveAnimClipNames [i] catch myDGV.rows.item [i-1].cells.item [0].value=""
					try myDGV.rows.item [i-1].cells.item [1].value = thisSourceObj.saveStartFrames[i] catch myDGV.rows.item [i-1].cells.item [1].value=1
					try myDGV.rows.item [i-1].cells.item [2].value = thisSourceObj.saveEndFrames[i] catch myDGV.rows.item [i-1].cells.item [2].value=2
					try myDGV.rows.item [i-1].cells.item [3].value = thisSourceObj.saveSkipFrames[i] catch myDGV.rows.item [i-1].cells.item [3].value=0
					try myDGV.rows.item [i-1].cells.item [6].value = thisSourceObj.saveStitchFinals[i] catch myDGV.rows.item [i-1].cells.item [6].value=false
					try myDGV.rows.item [i-1].cells.item [7].value = thisSourceObj.saveLoops[i] catch myDGV.rows.item [i-1].cells.item [7].value=false
					try myDGV.rows.item [i-1].cells.item [8].value = thisSourceObj.saveTransform[i] catch myDGV.rows.item [i-1].cells.item [8].value=false
					updateUI thisSourceObj i
					if i<=thisSourceObj.saveAnimClipNames.count then myDGV.CurrentCell =myDGV.rows.item [(i-1)].cells.item [1] 
					editCell 100 (i-1) awd_animation_rollout
				)
					
						
			)
			on btn_delete pressed do(
				deleteRow=myDGV.CurrentRow.Index+1
				myDGV.Rows.Remove(myDGV.CurrentRow)
				deleteItem  thisSourceObj.saveAnimClipNames deleteRow			
				deleteItem  thisSourceObj.saveAnimSourceIDs deleteRow
				deleteItem  thisSourceObj.saveStartFrames deleteRow
				deleteItem  thisSourceObj.saveEndFrames deleteRow
				deleteItem  thisSourceObj.saveSkipFrames deleteRow
				deleteItem  thisSourceObj.saveStitchFinals deleteRow	
				deleteItem  thisSourceObj.saveLoops deleteRow	
				deleteItem  thisSourceObj.saveTransform deleteRow	
				updateUI thisSourceObj (deleteRow-2)
			)
			on btn_close pressed do(
				destroyDialog awd_animation_rollout
			)
			on myDGV MouseUp  s e do (
				myDGV.commitEdit (dotNetClass "DataGridViewDataErrorContexts").commit
 				hit = s.HitTest e.x e.y
 				if (hit.RowIndex > -1) and (hit.ColumnIndex>5) do (
 					editCell hit.ColumnIndex hit.RowIndex awd_animation_rollout
				)		
			)
			on awd_animation_rollout close do (
				myDGV.commitEdit (dotNetClass "DataGridViewDataErrorContexts").commit
				for i=1 to thisSourceObj.saveAnimClipNames.Count do(
					updateRowFromUI (i-1) awd_animation_rollout
				)				
			)
			on awd_animation_rollout open do
			(				
				myCol1 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
				myCol1.headerText = "AWDAnimationClip-Name"
				myCol1.width = 50
				local myCol3 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
				myCol3.headerText = "Start"
				myCol3.width = 45
				local myCol4 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
				myCol4.headerText = "End"
				myCol4.width = 45
				local myCol5 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
				myCol5.headerText = "Skip"
				myCol5.width = 45
				local myCol6 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
				myCol6.headerText = "Duration(ms)"
				myCol6.width = 80
				myCol6.ReadOnly=true
				local myCol7 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
				myCol7.headerText = "Keys"
				myCol7.width =45
				myCol7.ReadOnly=true
				local myCol8 = dotNetObject "System.Windows.Forms.DataGridViewCheckBoxColumn"
				myCol8.headerText = "StichFinal"
				myCol8.width = 60
				local myCol9 = dotNetObject "System.Windows.Forms.DataGridViewCheckBoxColumn"
				myCol9.headerText = "Loop"
				myCol9.width = 45
				local myCol10 = dotNetObject "System.Windows.Forms.DataGridViewCheckBoxColumn"
				myCol10.headerText = "Transform"
				myCol10.width = 60
				myDGV.BackgroundColor = (dotNetClass "System.Drawing.Color").White    
				myDGV.BorderStyle=(dotNetClass "System.Windows.Forms.BorderStyle").NONE
				myDGV.AllowUserToAddRows=false
				myDGV.AllowUserToDeleteRows=false
				myDGV.AllowUserToResizeRows=false
				myDGV.AllowDrop =false
				--myDGV.ScrollBars= (dotNetObject "System.Windows.Forms.ScrollBars").Both
				--myDGV.MultiSelect=false
				myDGV.columns.add myCol1
				myDGV.columns.add myCol3
				myDGV.columns.add myCol4
				myDGV.columns.add myCol5
				myDGV.columns.add myCol6
				myDGV.columns.add myCol7
				myDGV.columns.add myCol8
				myDGV.columns.add myCol9
				myDGV.columns.add myCol10

				--myDGV.EditMode =  (dotNetClass "System.Windows.Forms.DataGridViewEditMode").EditOnEnter
				myDGV.width = 630
				myDGV.height = 283		
				resize [awd_animation_rollout.width, awd_animation_rollout.height]
			) 
	)
			
	new=#()
	newSources=#()
	availableSources=#()
 	thisIdx=0
	selectedIdx=-1
	newAnimSets=#()
	
 	for i in objects do(
 		for m in i.modifiers do (
 			if (classof m as string)=="AWDAnimationSet" then(
				append newAnimSets m
 			)
 			else if (classof m as string)=="AWDSkeleton" then(
 				append newSources (m.name+" - SkeletonAnimation")
				append availableSources m			
 				thisIdx+=1
				if thisAWDAnimSource==m then selectedIdx=thisIdx
 			)
 			else if (classof m as string)=="AWDVertexAnimSource" then(
 				append newSources (m.name+" - VertexAnimation")
				append availableSources m			
 				thisIdx+=1
				if thisAWDAnimSource==m then selectedIdx=thisIdx
 			)
 		)
 	)	
	
	allValidMaterials=#()
	for mat in meditMaterials do(
		if classof mat == Standardmaterial then appendIfUnique allValidMaterials mat
		else if classof mat == Multimaterial then (
			for matCnt in mat.materialIdList do (
				submat = mat[matCnt]
				if submat != undefined then 	if classof submat == Standardmaterial then appendIfUnique allValidMaterials submat
			)
		)
	)
	for mat in sceneMaterials do(
		if classof mat == Standardmaterial then appendIfUnique allValidMaterials mat
		else if classof mat == Multimaterial then (
			for matCnt in mat.materialIdList do (
				submat = mat[matCnt]
				if submat != undefined then 	if classof submat == Standardmaterial then appendIfUnique allValidMaterials submat
			)
		)
	)
-- 	numViews = sme.GetNumViews()
-- 	for numView=1 to numViews do (
-- 		oneView=sme.GetView numView
-- 		numNodes = oneView.GetNumNodes()
-- 		for numNode=1 to numNodes do(
-- 			thisNode= trackViewNodes[#sme][numView][numNode].reference
-- 			if classof mat == Standardmaterial then appendIfUnique allValidMaterials mat
-- 			else if classof mat == Multimaterial then (
-- 				for matCnt in mat.materialIdList do (
-- 					submat = mat[matCnt]
-- 					if submat != undefined then 	if classof submat == Standardmaterial then appendIfUnique allValidMaterials submat
-- 				)
-- 			)
-- 		)
-- 	)
				
	for mat in allValidMaterials do (	
		numMats=getNumSubTexmaps mat
		for i=1 to numMats do(
			thisTex=(getSubTexmap mat i)
			if classof thisTex == BitmapTexture then (
				hasUV=false
				try(
					thisAttr = custAttributes.get thisTex 1
					hasUV=thisAttr.uvanimeabled
				)
				catch()
				if hasUV then(
					append newSources (thisTex.name+" - UVAnimation")
					append availableSources thisTex
					if thisAWDAnimSource==thisTex then selectedIdx=newSources.Count
				)
			)
			else if classof thisTex ==  Normal_Bump then (
				if thisTex.normal_map!=undefined then(
					if classof thisTex.normal_map==BitmapTexture then (
						hasUV=false
						try(
							texmap=thisTex.normal_map
							thisAttr = custAttributes.get texmap 1
							hasUV=thisAttr.uvanimeabled
						)
						catch()
						if hasUV then(
							append newSources (thisTex.normal_map.name+" - UVAnimation")
							append availableSources thisTex.normal_map	
							if thisAWDAnimSource==thisTex.normal_map then selectedIdx=newSources.Count
						)
					)
				)
			)			
		)
	)	
	
	
		--TODO: Get all Textures with AWDTextureSettings and UVAnimEnabled and add them to the availableSources list!
	changeSelectedIdx=-1
	doInit=true
	if thisMode=="Vertex" or thisMode=="Skeleton" then (
	-- if this is a clone and not the original source, we first get the orginal source before init
		if classof thisAWDAnimSource as string == "AWDSkeletonClone" or classof thisAWDAnimSource as string == "AWDVertexAnimSourceClone" then (
			doInit=false
			if thisAWDAnimSource.target_ID =="noIDSet" then (
				if classof thisAWDAnimSource as string == "AWDSkeletonClone" then messageBox ("Clipmanager can not be opened !!!\n\n A AWDSkeletonClone must be assigned to a AWDSkeleton")
				if classof thisAWDAnimSource as string == "AWDVertexAnimSourceClone" then messageBox ("Clipmanager can not be opened !!!\n\n A AWDVertexAnimClone must be assigned to a AWDVertexAnim")
				
				return()
			)
			else(
				idx=0
				for s in availableSources do(
					idx+=1
					if s.thisAWDID==thisAWDAnimSource.target_ID then(
						thisSourceObj=s
						doInit=true
						changeSelectedIdx=idx
						exit
					)
				)
			)
		)
	)
	if(selectedIdx<=0) and (changeSelectedIdx<=0) then (
		messageBox ("Error - could not open the AWDAnimationClipManager for this animSource.")
		return()
	)
	try destroyDialog awd_animation_rollout catch ()
	createDialog awd_animation_rollout style:#(#style_titlebar, #style_border, #style_sysmenu, #style_minimizebox, #style_resizing)
	if dialogPosition!=[0,0] then SetDialogPos awd_animation_rollout dialogPosition
	awd_animation_rollout.width=dialogWidth;
	awd_animation_rollout.height=dialogHeight;
	if doInit then initSource awd_animation_rollout
	else (
		messageBox ("Error - when opening the clipmanager")
		try destroyDialog awd_animation_rollout catch ()
		return()
	)
	-- if changeSelectedIdx is > 0 then a Clone was selected initially, and we select the clone in the list.
	if changeSelectedIdx>0 then (
		awd_animation_rollout.selectedSource.selection=changeSelectedIdx
		selectedIdx=1
		for clone in availableClones do(
			if clone==thisAWDAnimSource then (
				awd_animation_rollout.selectedClone.selection=selectedIdx
				exit
			)
			selectedIdx+=1
		)
		thisSourceObj=thisAWDAnimSource
		initClonedSource awd_animation_rollout
	)
	awd_animation_rollout.selectedSource.items=newSources
	if (selectedIdx>0) then (
		if awd_animation_rollout.selectedSource.items.count>0 and selectedIdx<=awd_animation_rollout.selectedSource.items.count then(
			awd_animation_rollout.selectedSource.selection=selectedIdx
		)
	)
	if animNr>=0 and animNr<awd_animation_rollout.myDGV.rows.count then(
		awd_animation_rollout.myDGV.CurrentCell = awd_animation_rollout.myDGV.rows.item [animNr].cells.item [0] 
	)
)
