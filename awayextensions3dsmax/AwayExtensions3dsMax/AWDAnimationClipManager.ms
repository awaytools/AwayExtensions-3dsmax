	
	fn getAWDID = (
		newTime=getUniversalTime()
		newString="awd_"+newTime[1] as string+newTime[2] as string+newTime[3] as string+newTime[4] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+random 0.0 10000.0 as string
		return newString
	)	
	fn initSource  myDGV2 lb_AvailableAnimSets2 lb_AssignedAnimSets2 clonedList simpleModeCBX myColum1 btn1 btn2 btn3 btn4 = (
		global thisSourceObj
		global allAnimSetsSimple=#()
		global allAnimSets=#()
		rowCnt=myDGV2.rows.count-1
		for i=0 to rowCnt do myDGV2.Rows.Remove(myDGV2.rows.item[0])		
		newNames=#()
		for animSet in thisSourceObj.saveAnimSets do(
			append newNames animSet.name
			append allAnimSetsSimple true
		)
		if thisSourceObj.simpleMode then(
			simpleModeCBX.state=true		
			myDGV2.width=820
			myColum1.width=390
			btn1.visible=false			
			btn2.visible=false			
			btn3.visible=false			
			btn4.visible=false			
			lb_AvailableAnimSets2.visible=false			
			lb_AssignedAnimSets2.visible=false			
		)
		else (
			simpleModeCBX.state=false
			myDGV2.width=630
			myColum1.width=200
			btn1.visible=true			
			btn2.visible=true			
			btn3.visible=true			
			btn4.visible=true			
			lb_AvailableAnimSets2.visible=true			
			lb_AssignedAnimSets2.visible=true				
		)
		clonedList.items=#("original")
		availAbleClones=#(thisSourceObj)
		for i=1 to thisSourceObj.savedClones.count do
		(
			append availAbleClones thisSourceObj.savedClones[i]
			clonedList.items= append clonedList.items thisSourceObj.savedClones[i].name
		)
		clonedList.selection=1
		for i=1 to thisSourceObj.saveAnimClipNames.count do
		(
			myDGV2.rows.add ()
			try myDGV2.rows.item [i-1].cells.item [0].value = thisSourceObj.saveAnimClipNames [i] catch myDGV2.rows.item [i-1].cells.item [0].value=""
			try myDGV2.rows.item [i-1].cells.item [1].value = thisSourceObj.saveStartFrames[i] catch myDGV2.rows.item [i-1].cells.item [1].value=1
			try myDGV2.rows.item [i-1].cells.item [2].value = thisSourceObj.saveEndFrames[i] catch myDGV2.rows.item [i-1].cells.item [2].value=2
			try myDGV2.rows.item [i-1].cells.item [3].value = thisSourceObj.saveSkipFrames[i] catch myDGV2.rows.item [i-1].cells.item [3].value=0
			try myDGV2.rows.item [i-1].cells.item [6].value = thisSourceObj.saveStitchFinals[i] catch myDGV2.rows.item [i-1].cells.item [6].value=false
			try(
				myDGV2.rows.item[i-1].Cells.Item[4].value	= 	((thisSourceObj.saveEndFrames[i]-thisSourceObj.saveStartFrames[i])*((1.0/frameRate) as float)*1000) as string
				myDGV2.rows.item[i-1].Cells.Item[5].value	= ((thisSourceObj.saveEndFrames[i]-thisSourceObj.saveStartFrames[i])/(thisSourceObj.saveSkipFrames[i]+1)) as string
			)
			catch()
			newlist=#()
			cntIdx=1
			for isSimple in allAnimSetsSimple do(				
				if isSimple then append newlist thisSourceObj.saveAnimSets[cntIdx].name
				--else -- to do add support for "not simple" mode :)
				cntIdx+=1
			)
			append allAnimSets newlist						
		)
		lb_AvailableAnimSets2.items=newNames
		lb_AssignedAnimSets2.items=#()
		--if thisSourceObj.saveAnimClipNames.count>0 then if (animNr+1)<thisSourceObj.saveAnimClipNames.count then myDGV2.CurrentCell =myDGV2.rows.item [(animNr+1)].cells.item [0] 

		if allAnimSets != undefined then (
			if (myDGV2.CurrentRow!=undefined) then (
				if allAnimSets.Count>=(myDGV2.CurrentRow.Index+1) then(
					lb_AssignedAnimSets2.items=allAnimSets[(myDGV2.CurrentRow.Index+1)]
				)
			)
		)
	)
	
	fn initClonedSource  myDGV2 = (
		global thisSourceObj
		global allAnimSetsSimple=#()
		global allAnimSets=#()
		rowCnt=myDGV2.rows.count-1
		for i=0 to rowCnt do myDGV2.Rows.Remove(myDGV2.rows.item[0])		
		for i=1 to thisSourceObj.saveAnimClipNames.count do
		(
			myDGV2.rows.add ()
			try myDGV2.rows.item [i-1].cells.item [0].value = thisSourceObj.saveAnimClipNames [i] catch myDGV2.rows.item [i-1].cells.item [0].value=""
			try myDGV2.rows.item [i-1].cells.item [1].value = thisSourceObj.saveStartFrames[i] catch myDGV2.rows.item [i-1].cells.item [1].value=1
			try myDGV2.rows.item [i-1].cells.item [2].value = thisSourceObj.saveEndFrames[i] catch myDGV2.rows.item [i-1].cells.item [2].value=2
			try myDGV2.rows.item [i-1].cells.item [3].value = thisSourceObj.saveSkipFrames[i] catch myDGV2.rows.item [i-1].cells.item [3].value=0
			try myDGV2.rows.item [i-1].cells.item [6].value = thisSourceObj.saveStitchFinals[i] catch myDGV2.rows.item [i-1].cells.item [6].value=false
			try(
				myDGV2.rows.item[i-1].Cells.Item[4].value	= 	((thisSourceObj.saveEndFrames[i]-thisSourceObj.saveStartFrames[i])*((1.0/frameRate) as float)*1000) as string
				myDGV2.rows.item[i-1].Cells.Item[5].value	= ((thisSourceObj.saveEndFrames[i]-thisSourceObj.saveStartFrames[i])/(thisSourceObj.saveSkipFrames[i]+1) as integer) as string
			)
			catch()				
		)
		--if thisSourceObj.saveAnimClipNames.count>0 then if (animNr+1)<thisSourceObj.saveAnimClipNames.count then myDGV2.CurrentCell =myDGV2.rows.item [(animNr+1)].cells.item [0] 
	)
	
	fn validateClips thisSourceObj= (
		valid=true
		if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveAnimSourceIDs.count then valid=false
		if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveStartFrames.count then valid=false
		if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveEndFrames.count then valid=false
		if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveSkipFrames.count then valid=false
		if thisSourceObj.saveAnimClipNames.count!=thisSourceObj.saveStitchFinals.count then valid=false		
		if not valid then(	
			minCount=99999999
			if thisSourceObj.saveAnimClipNames.count<minCount then minCount=thisSourceObj.saveAnimClipNames.count
			if thisSourceObj.saveAnimSourceIDs.count<minCount then minCount=thisSourceObj.saveAnimSourceIDs.count
			if thisSourceObj.saveStartFrames.count<minCount then minCount=thisSourceObj.saveStartFrames.count
			if thisSourceObj.saveEndFrames.count<minCount then minCount=thisSourceObj.saveEndFrames.count
			if thisSourceObj.saveSkipFrames.count<minCount then minCount=thisSourceObj.saveSkipFrames.count
			if thisSourceObj.saveStitchFinals.count<minCount then minCount=thisSourceObj.saveStitchFinals.count				
			saveAnimClipNames=#()
			saveAnimSourceIDs=#()
			saveStartFrames=#()
			saveEndFrames=#()
			saveSkipFrames=#()
			saveStitchFinals=#()
			for i=1 to minCount do(	
				append saveAnimClipNames thisSourceObj.saveAnimClipNames[i]
				append saveAnimSourceIDs thisSourceObj.saveAnimSourceIDs[i]
				append saveStartFrames thisSourceObj.saveStartFrames[i]
				append saveEndFrames thisSourceObj.saveEndFrames[i]
				append saveSkipFrames thisSourceObj.saveSkipFrames[i]
				append saveStitchFinals thisSourceObj.saveStitchFinals[i]
			)
			thisSourceObj.saveAnimClipNames=saveAnimClipNames
			thisSourceObj.saveAnimSourceIDs=saveAnimSourceIDs
			thisSourceObj.saveStartFrames=saveStartFrames
			thisSourceObj.saveEndFrames=saveEndFrames
			thisSourceObj.saveSkipFrames=saveSkipFrames
			thisSourceObj.saveStitchFinals=saveStitchFinals			
		)
	)
		
	fn openAWDClipManager thisAWDAnimSource animType animNrInput= (
		clearListener ()
		global thisSourceObj=thisAWDAnimSource
		global animNr=animNrInput-1
		validateClips thisSourceObj
		global availableSources=#()
		global availAbleClones=#()
		global clipBoard=#()
		try (destroyDialog animClipManager_roll) catch ()
		rollout animClipManager_roll "AWD Animation Clip Manager" width:840 height:400
		(
				label lbl_selectedSource "AnimationSource:" pos:[10,15]	
				dropDownList selectedSource "" items:#("sdas", "adas") width:200 pos:[100,10]	
				label lbl_selectedClone "Clone:" pos:[310,15]	
				dropDownList selectedClone "" items:#("sdas", "adas") width:200  pos:[345,10]	
				checkBox cbx_simple_mode "Simple Mode" pos:[570,14]	 enabled:false
				button btn_select_in_modPanel "Select in Mod-Panel" pos:[670,11]	
				listBox lb_AvailableAnimSets " Available AnimationSets" items:#("sdas", "adas") width:180  height:8 pos:[650,45]	
				button btn_assignAnimSet "assign"  width:60 height:30  pos:[670,180]	
				button btn_removeAnimSet "remove"  width:60 height:30  pos:[755,180]	
				label lbl_AnimClips "AnimationClips" pos:[10,45]	
				listBox lb_AssignedAnimSets " Assigned AnimationSets" items:#("sdas", "adas") width:180  height:8 pos:[650,218]	

				dotNetControl myDGV "System.Windows.Forms.DataGridView"	pos:[10,62]	
				button btn_addAnimClip "Add New Clip" pos:[10,360] height:30
				button btn_preventOverlapping "Prevent overlapping clips" pos:[100,360] height:30 enabled:false
				button btn_delete "Delete" pos:[250,360] height:30
				button btn_newAnimSet "New" pos:[650,360] height:30			
				button btn_selectAnimSet "Select" pos:[700,360] height:30
				--label lbl_clipboard "ClipBoard = 0 AnimationClips"
				--button btn_copyClip "Copy" pos:[340,360] width:54 height:30
				--button btn_copyAddClip "Copy +" pos:[400,360] width:54 height:30
				--button btn_cutClip "Cut" pos:[460,360] width:54 height:30
				--button btn_pasteClip "Paste" pos:[520,360] width:54 height:30
				--button btn_clearClip "Clear" pos:[580,360] width:54 height:30
				
				--on btn_copyClip pressed do (
				--)
				--on btn_copyAddClip pressed do (
				--)
				--on btn_cutClip pressed do (
				--)
				--on btn_pasteClip pressed do (
				--	for clip in clipBoard do(
				--		if clip.Count!=6 then (
				--			messageBox("ClipBoard corrupt. Cannot paste.")
				--			return()
				--		)
-- 						else (
-- 							irowCnt=myDGV.rows.count+1
-- 							theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDAnimationClip #"+irowCnt as string)
-- 							theobj.text ="Add name for new AWDAnimationClip"
-- 							DialogResult = theObj.Showmodal()
-- 							dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
-- 							result = theobj.InstanceName
-- 							print result
-- 							myDGV.rows.add ()
-- 							i=myDGV.rows.count
-- 							append thisSourceObj.saveAnimClipNames result	
-- 							append thisSourceObj.saveAnimSourceIDs (getAWDID() as string)
-- 							try thisVal=(myDGV.rows.item [i-2].cells.item [1].value+100) catch thisVal=0		
-- 							append thisSourceObj.saveStartFrames thisVal
-- 							try thisVal=(myDGV.rows.item [i-2].cells.item [2].value+100) catch thisVal=100
-- 							append thisSourceObj.saveEndFrames thisVal
-- 							append thisSourceObj.saveSkipFrames 0
-- 							append thisSourceObj.saveStitchFinals false
-- 							try myDGV.rows.item [i-1].cells.item [0].value = thisSourceObj.saveAnimClipNames [i] catch myDGV.rows.item [i-1].cells.item [0].value=""
-- 							try myDGV.rows.item [i-1].cells.item [1].value = thisSourceObj.saveStartFrames[i] catch myDGV.rows.item [i-1].cells.item [1].value=1
-- 							try myDGV.rows.item [i-1].cells.item [2].value = thisSourceObj.saveEndFrames[i] catch myDGV.rows.item [i-1].cells.item [2].value=2
-- 							try myDGV.rows.item [i-1].cells.item [3].value = thisSourceObj.saveSkipFrames[i] catch myDGV.rows.item [i-1].cells.item [3].value=0
-- 							try myDGV.rows.item [i-1].cells.item [6].value = thisSourceObj.saveStitchFinals[i] catch myDGV.rows.item [i-1].cells.item [6].value=false
-- 							thisSourceObj.updateUI thisSourceObj i
-- 							if i<=thisSourceObj.saveAnimClipNames.count then myDGV.CurrentCell =myDGV.rows.item [(i-1)].cells.item [1] 
-- 								)
-- 							)
-- 				)
-- 				on btn_clearClip pressed do (
-- 					clipBoard=#()
-- 					append clipBoard #() --clip-modifier
-- 					append clipBoard #() --clip-name
-- 					append clipBoard #() --clip-id
-- 					append clipBoard #() --clip-startFrame
-- 					append clipBoard #() --clip-endFrame
-- 					append clipBoard #() --clip-skipFrame
-- 					append clipBoard #() --clip-stitchFinal
-- 				)
				on btn_select_in_modPanel pressed do (
					if selectedClone.selection==1 then (
						max modify mode 
						modPanel.setCurrentObject availableSources[selectedSource.selection]
					)
					else if selectedClone.selection>1 then (
						max modify mode 
						modPanel.setCurrentObject availAbleClones[selectedClone.selection]
					)
					
				)
				
				local dragStart=false
				local lastDragY=0
				local	selectedCells=#()
				local myCol1
				on cbx_simple_mode changed thisValue do(
					availableSources[selectedSource.selection].simpleMode=thisValue
					if thisValue then (
						myDGV.width=820
						myCol1.width = 390
						lb_AvailableAnimSets.visible=false
						btn_assignAnimSet.visible=false
						btn_removeAnimSet.visible=false
						lb_AssignedAnimSets.visible=false
						btn_newAnimSet.visible=false
						btn_selectAnimSet.visible=false
					)
					else(
						myDGV.width=630
						myCol1.width = 200
						lb_AvailableAnimSets.visible=true
						btn_assignAnimSet.visible=true
						btn_removeAnimSet.visible=true
						lb_AssignedAnimSets.visible=true
						btn_newAnimSet.visible=true
						btn_selectAnimSet.visible=true
					)
				)
				on animClipManager_roll open do
				(					
					myCol1 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
					myCol1.headerText = "AWDAnimationClip-Name"
					myCol1.width = 200
					local myCol3 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
					myCol3.headerText = "Start"
					myCol3.width = 50
					local myCol4 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
					myCol4.headerText = "End"
					myCol4.width = 50
					local myCol5 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
					myCol5.headerText = "Skip"
					myCol5.width = 50
					local myCol6 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
					myCol6.headerText = "Duration(ms)"
					myCol6.width = 80
					myCol6.ReadOnly=true
					local myCol7 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
					myCol7.headerText = "Exported Keys"
					myCol7.width = 85
					myCol7.ReadOnly=true
					local myCol8 = dotNetObject "System.Windows.Forms.DataGridViewCheckBoxColumn"
					myCol8.headerText = "StichFinal"
					myCol8.width = 60
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

					--myDGV.EditMode =  (dotNetClass "System.Windows.Forms.DataGridViewEditMode").EditOnEnter
					myDGV.width = 630
					myDGV.height = 283
					curMod=modPanel.GetCurrentObject()
					global thisSourceObj=curMod
					changeSelectedIdx=-1
					doInit=true
					-- if this is a clone and not the original source, we first get the orginal source before init
					if classof curMod as string == "AWDSkeletonClone" or classof curMod as string == "AWDVertexAnimSourceClone" then (
						print "shit"
						print (classof curMod as string)
						doInit=false
						print (curMod.target_ID as string)
						if curMod.target_ID =="noIDSet" then (
							if classof curMod as string == "AWDSkeletonClone" then messageBox ("Clipmanager can not be opened !!!\n\n A AWDSkeletonClone must be assigned to a AWDSkeleton")
							if classof curMod as string == "AWDVertexAnimSourceClone" then messageBox ("Clipmanager can not be opened !!!\n\n A AWDVertexAnimClone must be assigned to a AWDVertexAnim")
							destroyDialog animClipManager_roll
						)
						else(
							idx=0
							for s in availableSources do(
								idx+=1
								if s.thisAWDID==curMod.target_ID then(
									thisSourceObj=s
									doInit=true
									changeSelectedIdx=idx
									exit
								)
							)
						)
					)
					if doInit then initSource myDGV lb_AvailableAnimSets lb_AssignedAnimSets selectedClone cbx_simple_mode myCol1 btn_assignAnimSet btn_removeAnimSet btn_newAnimSet btn_selectAnimSet
					-- if changeSelectedIdx is > 0 then a Clone was selected initially, and we select the clone in the list.
					if changeSelectedIdx>0 then (
						selectedSource.selection=changeSelectedIdx
						thisSourceObj=curMod
						print thisSourceObj.name
						selectedIdx=1
						print availAbleClones
						for clone in availAbleClones do(
							if clone==curMod then (
								selectedClone.selection=selectedIdx
								exit
							)
							selectedIdx+=1
						)
						initClonedSource myDGV
					)				
				) 
								
				on selectedClone selected thisValue do(
					global thisSourceObj=availAbleClones[thisValue]
					initClonedSource myDGV
				)
				on selectedSource selected thisValue do(
					global thisSourceObj=availableSources[thisValue]
					initSource myDGV lb_AvailableAnimSets lb_AssignedAnimSets selectedClone cbx_simple_mode  myCol1 btn_assignAnimSet btn_removeAnimSet btn_newAnimSet btn_selectAnimSet
				)
				on myDGV SelectionChanged e do ( 
					if (myDGV.CurrentRow!=undefined) then (				
						global allAnimSets
						if allAnimSets != undefined then (
							if allAnimSets.Count>=(myDGV.CurrentRow.Index+1) then(
								lb_AssignedAnimSets.items=allAnimSets[(myDGV.CurrentRow.Index+1)]
							)
						)
					)
				)
				on myDGV CellEndEdit s e do
				(
				--format "cell end edit... : %\n" s.rows.item[e.RowIndex].cells.item[e.ColumnIndex].value
					if e.ColumnIndex==0 then thisSourceObj.saveAnimClipNames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [0].value
					--if e.ColumnIndex==2 then thisSourceObj.saveAnimClipNames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [2].value
					if e.ColumnIndex==1 then (
						oldValue=thisSourceObj.saveStartFrames[e.RowIndex+1]
						try thisSourceObj.saveStartFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [1].value as integer catch(
							thisSourceObj.saveStartFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [1].value=oldValue
						)
						
					)
					if e.ColumnIndex==2 then (
						oldValue=thisSourceObj.saveEndFrames[e.RowIndex+1]
						try thisSourceObj.saveEndFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [2].value as integer catch(
							thisSourceObj.saveEndFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [2].value=oldValue
						)
					)
					if e.ColumnIndex==3 then (
						oldValue=thisSourceObj.saveEndFrames[e.RowIndex+1]
						try thisSourceObj.saveSkipFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [3].value as float catch(
							thisSourceObj.saveSkipFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [3].value=oldValue
						)
					)
					if e.ColumnIndex==6 then thisSourceObj.saveStitchFinals[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [6].value
					try(
						myDGV.rows.item[e.RowIndex].Cells.Item[4].value	= 	((thisSourceObj.saveEndFrames[e.RowIndex+1]-thisSourceObj.saveStartFrames[e.RowIndex+1])*((1.0/frameRate) as float)*1000) as string
						myDGV.rows.item[e.RowIndex].Cells.Item[5].value	= ((thisSourceObj.saveEndFrames[e.RowIndex+1]-thisSourceObj.saveStartFrames[e.RowIndex+1])/(thisSourceObj.saveSkipFrames[e.RowIndex+1]+1) as integer) as string
					)
					catch()
					thisSourceObj.updateUI thisSourceObj e.RowIndex
				)
				on btn_preventOverlapping pressed do (
					messageBox ("This function is not implemented yet. It will check if any clips overlap, and offer to fix automaticly.")
				)
				on btn_addAnimClip pressed do (
					irowCnt=myDGV.rows.count+1
					theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDAnimationClip #"+irowCnt as string)
					theobj.text ="Add name for new AWDAnimationClip"
					DialogResult = theObj.Showmodal()
					dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
					result = theobj.InstanceName
					
					myDGV.rows.add ()
					i=myDGV.rows.count
					append thisSourceObj.saveAnimClipNames result	
					append thisSourceObj.saveAnimSourceIDs (getAWDID() as string)
					try thisVal=(myDGV.rows.item [i-2].cells.item [1].value+100) catch thisVal=0		
					append thisSourceObj.saveStartFrames thisVal
					try thisVal=(myDGV.rows.item [i-2].cells.item [2].value+100) catch thisVal=100
					append thisSourceObj.saveEndFrames thisVal
					append thisSourceObj.saveSkipFrames 0
					append thisSourceObj.saveStitchFinals false
					try myDGV.rows.item [i-1].cells.item [0].value = thisSourceObj.saveAnimClipNames [i] catch myDGV.rows.item [i-1].cells.item [0].value=""
					try myDGV.rows.item [i-1].cells.item [1].value = thisSourceObj.saveStartFrames[i] catch myDGV.rows.item [i-1].cells.item [1].value=1
					try myDGV.rows.item [i-1].cells.item [2].value = thisSourceObj.saveEndFrames[i] catch myDGV.rows.item [i-1].cells.item [2].value=2
					try myDGV.rows.item [i-1].cells.item [3].value = thisSourceObj.saveSkipFrames[i] catch myDGV.rows.item [i-1].cells.item [3].value=0
					try myDGV.rows.item [i-1].cells.item [6].value = thisSourceObj.saveStitchFinals[i] catch myDGV.rows.item [i-1].cells.item [6].value=false
					thisSourceObj.updateUI thisSourceObj i
					if i<=thisSourceObj.saveAnimClipNames.count then myDGV.CurrentCell =myDGV.rows.item [(i-1)].cells.item [1] 
					
						
				)
				on btn_delete pressed do(
					print (myDGV.CurrentRow.Index as string)
					deleteRow=myDGV.CurrentRow.Index+1
					myDGV.Rows.Remove(myDGV.CurrentRow)
					deleteItem  thisSourceObj.saveAnimClipNames deleteRow			
					deleteItem  thisSourceObj.saveAnimSourceIDs deleteRow
					deleteItem  thisSourceObj.saveStartFrames deleteRow
					deleteItem  thisSourceObj.saveEndFrames deleteRow
					deleteItem  thisSourceObj.saveSkipFrames deleteRow
					deleteItem  thisSourceObj.saveStitchFinals deleteRow			
					thisSourceObj.updateUI thisSourceObj (deleteRow-2)	
				)
				on btn_close pressed do(
					destroyDialog animClipManager_roll
				)

			)
			
 			newSources=#()
			availableSources=#()
 			thisIdx=0
			selectedIdx=-1
			curMod=modPanel.GetCurrentObject()
 			for i in objects do(
 				for m in i.modifiers do (
 					if (classof m as string)=="AWDSkeleton" or (classof m as string)=="AWDVertexAnimSource" then(
 						append newSources m.name
						append availableSources m			
 						thisIdx+=1
						if curMod==m then selectedIdx=thisIdx
 					)
 				)
 			)		
			
			try destroyDialog animClipManager_roll catch (print ("couldfind it"))
			createDialog animClipManager_roll
				
			animClipManager_roll.selectedSource.items=newSources
			if (selectedIdx>0) then (
				if animClipManager_roll.selectedSource.items.count>0 and selectedIdx<=animClipManager_roll.selectedSource.items.count then(
					animClipManager_roll.selectedSource.selection=selectedIdx
				)
			)

		
	)