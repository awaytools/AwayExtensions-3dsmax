	
	fn DOTNETtoMXSRGB dnClr = (local clr ; clr = if classof dnClr.r == Integer do (color dnClr.r dnClr.g dnClr.b) ; clr)
	
	fn openAWDSetManager thisAWDAnimSet = (
		clearListener ()
		global thisAnimSet=thisAWDAnimSet
		valid=true
		if thisAnimSet.saveAnimClipNames.count!=thisAnimSet.saveActiveStates.count then valid=false
		if thisAnimSet.saveAnimClipNames.count!=thisAnimSet.saveAnimSourceIDs.count then valid=false
		if thisAnimSet.saveAnimClipNames.count!=thisAnimSet.saveStartFrames.count then valid=false
		if thisAnimSet.saveAnimClipNames.count!=thisAnimSet.saveEndFrames.count then valid=false
		if thisAnimSet.saveAnimClipNames.count!=thisAnimSet.saveSkipFrames.count then valid=false
		if thisAnimSet.saveAnimClipNames.count!=thisAnimSet.saveStitchFinals.count then valid=false
		
		if not valid then(	
			maxCount=0
			minCount=100000
			if thisAnimSet.saveAnimClipNames.count<minCount then minCount=thisAnimSet.saveAnimClipNames.count
			if thisAnimSet.saveActiveStates.count<minCount then minCount=thisAnimSet.saveActiveStates.count
			if thisAnimSet.saveAnimSourceIDs.count<minCount then minCount=thisAnimSet.saveAnimSourceIDs.count
			if thisAnimSet.saveStartFrames.count<minCount then minCount=thisAnimSet.saveStartFrames.count
			if thisAnimSet.saveEndFrames.count<minCount then minCount=thisAnimSet.saveEndFrames.count
			if thisAnimSet.saveSkipFrames.count<minCount then minCount=thisAnimSet.saveSkipFrames.count
			if thisAnimSet.saveStitchFinals.count<minCount then minCount=thisAnimSet.saveStitchFinals.count
				
			if thisAnimSet.saveActiveStates.count>maxCount then maxCount=thisAnimSet.saveActiveStates.count
			if thisAnimSet.saveAnimClipNames.count>maxCount then maxCount=thisAnimSet.saveAnimClipNames.count
			if thisAnimSet.saveAnimSourceIDs.count>maxCount then maxCount=thisAnimSet.saveAnimSourceIDs.count
			if thisAnimSet.saveStartFrames.count>maxCount then maxCount=thisAnimSet.saveStartFrames.count
			if thisAnimSet.saveEndFrames.count>maxCount then maxCount=thisAnimSet.saveEndFrames.count
			if thisAnimSet.saveSkipFrames.count>maxCount then maxCount=thisAnimSet.saveSkipFrames.count
			if thisAnimSet.saveStitchFinals.count>maxCount then maxCount=thisAnimSet.saveStitchFinals.count
			resolveMode=1
			if resolveMode==1 then (
				thisAnimSet.saveActiveStates=#()
				thisAnimSet.saveAnimClipNames=#()
				thisAnimSet.saveAnimSourceIDs=#()
				thisAnimSet.saveStartFrames=#()
				thisAnimSet.saveEndFrames=#()
				thisAnimSet.saveSkipFrames=#()
				thisAnimSet.saveStitchFinals=#()
			)
			if resolveMode==2 then (	
				saveActiveStates=#()
				saveAnimClipNames=#()
				saveAnimSourceIDs=#()
				saveStartFrames=#()
				saveEndFrames=#()
				saveSkipFrames=#()
				saveStitchFinals=#()
				for i=1 to minCount do(	
					append saveActiveStates thisAnimSet.saveActiveStates[i]
					append saveAnimClipNames thisAnimSet.saveAnimClipNames[i]
					append saveAnimSourceIDs thisAnimSet.saveAnimSourceIDs[i]
					append saveStartFrames thisAnimSet.saveStartFrames[i]
					append saveEndFrames thisAnimSet.saveEndFrames[i]
					append saveSkipFrames thisAnimSet.saveSkipFrames[i]
					append saveStitchFinals thisAnimSet.saveStitchFinals[i]
				)
				thisAnimSet.saveActiveStates=saveActiveStates
				thisAnimSet.saveAnimClipNames=saveAnimClipNames
				thisAnimSet.saveAnimSourceIDs=saveAnimSourceIDs
				thisAnimSet.saveStartFrames=saveStartFrames
				thisAnimSet.saveEndFrames=saveEndFrames
				thisAnimSet.saveSkipFrames=saveSkipFrames
				thisAnimSet.saveStitchFinals=saveStitchFinals
			)
			if resolveMode==3 then (	
				saveActiveStates=#()
				saveAnimClipNames=#()
				saveAnimSourceIDs=#()
				saveStartFrames=#()
				saveEndFrames=#()
				saveSkipFrames=#()
				saveStitchFinals=#()
				for i=1 to maxCount do(	
					if thisAnimSet.saveActiveStates.Count<=i then append saveActiveStates thisAnimSet.saveActiveStates[i]
					else append saveActiveStates false
					if thisAnimSet.saveAnimClipNames.Count<=i then append saveAnimClipNames thisAnimSet.saveAnimClipNames[i]
					else append saveAnimClipNames ""
					if thisAnimSet.saveAnimSourceIDs.Count<=i then append saveAnimSourceIDs thisAnimSet.saveAnimSourceIDs[i]
					else append saveAnimSourceIDs ""
					--if thisAnimSet.saveAnimSourceNames.Count<=i then append saveAnimSourceNames thisAnimSet.saveAnimSourceNames[i]
					--else append saveAnimSourceNames ""
					if thisAnimSet.saveStartFrames.Count<=i then append saveStartFrames thisAnimSet.saveStartFrames[i]
					else append saveStartFrames 1
					if thisAnimSet.saveEndFrames.Count<=i then append saveEndFrames thisAnimSet.saveEndFrames[i]
					else append saveEndFrames 2
					if thisAnimSet.saveSkipFrames.Count<=i then append saveSkipFrames thisAnimSet.saveSkipFrames[i]
					else append saveSkipFrames 0
					if thisAnimSet.saveStitchFinals.Count<=i then append saveStitchFinals thisAnimSet.saveStitchFinals[i]
					else append saveStitchFinals false
				)
				thisAnimSet.saveActiveStates=saveActiveStates
				thisAnimSet.saveAnimClipNames=saveAnimClipNames
				thisAnimSet.saveAnimSourceIDs=saveAnimSourceIDs
				thisAnimSet.saveStartFrames=saveStartFrames
				thisAnimSet.saveEndFrames=saveEndFrames
				thisAnimSet.saveSkipFrames=saveSkipFrames
				thisAnimSet.saveStitchFinals=saveStitchFinals
			)
				
			print "AWDCLIPS ERROR = "
		)
			try (destroyDialog animClipManager_roll) catch ()
			rollout animClipManager_roll "AWD Animation Clip Manager" width:840 height:400
			(
				label lbl_1 ""
				editText animSetName "AWDAnimationSet-Name" across:3 
				radiobuttons rdoGrp_animType "" labels:#("Skeleton", "Vertex")
				editText animationSource "AnimationSource" 
				label lbl_2 ""
				dotNetControl myDGV "System.Windows.Forms.DataGridView"	pos:[10,50]	
				button btn_addAnimClip "Add CLip" pos:[10,360]
				on btn_addAnimClip pressed do (
					myDGV.rows.add ()
				)
				local dragStart=false
				local lastDragY=0
				local	selectedCells=#()
				on animClipManager_roll open do
				(
					
					local myCol02 = dotNetObject "System.Windows.Forms.DataGridViewCheckBoxColumn"
					myCol02.headerText = "Use"
					myCol02.width = 30
					local myCol1 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
					myCol1.headerText = "AWDAnimationClip-Name"
					myCol1.width = 200
					local myCol2 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
					myCol2.headerText = "Animation-Source"
					myCol2.width = 137
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
					myCol6.headerText = "Duration(sec)"
					myCol6.width = 80
					myCol6.ReadOnly=true
					local myCol7 = dotNetObject "System.Windows.Forms.DataGridViewTextBoxColumn"
					myCol7.headerText = "Length"
					myCol7.width = 80
					myCol7.ReadOnly=true
					local myCol8 = dotNetObject "System.Windows.Forms.DataGridViewCheckBoxColumn"
					myCol8.headerText = "StichFinal"
					myCol8.width = 100
					myDGV.BackgroundColor = (dotNetClass "System.Drawing.Color").Black    
					myDGV.BorderStyle=(dotNetClass "System.Windows.Forms.BorderStyle").NONE
					myDGV.AllowUserToAddRows=false
					myDGV.AllowUserToDeleteRows=false
					myDGV.AllowUserToResizeRows=false
					myDGV.AllowDrop =false
					--myDGV.ScrollBars= (dotNetObject "System.Windows.Forms.ScrollBars").Both
					--myDGV.MultiSelect=false
					myDGV.columns.add myCol02
					myDGV.columns.add myCol1
					myDGV.columns.add myCol2
					myDGV.columns.add myCol3
					myDGV.columns.add myCol4
					myDGV.columns.add myCol5
					myDGV.columns.add myCol6
					myDGV.columns.add myCol7
					myDGV.columns.add myCol8

					myDGV.width = 820
					myDGV.height = 300

					for i=1 to thisAnimSet.saveAnimClipNames.count do
					(
						myDGV.rows.add ()
						try myDGV.rows.item [i-1].cells.item [0].value = thisAnimSet.saveActiveStates[i] catch myDGV.rows.item [i-1].cells.item [0].value=false
						try myDGV.rows.item [i-1].cells.item [1].value = thisAnimSet.saveAnimClipNames [i] catch myDGV.rows.item [i-1].cells.item [1].value=""
						--try myDGV.rows.item [i-1].cells.item [2].value = thisAnimSet.saveAnimSourceIDs[i] catch myDGV.rows.item [i-1].cells.item [2].value=""
						try myDGV.rows.item [i-1].cells.item [3].value = thisAnimSet.saveStartFrames[i] catch myDGV.rows.item [i-1].cells.item [3].value=1
						try myDGV.rows.item [i-1].cells.item [4].value = thisAnimSet.saveEndFrames[i] catch myDGV.rows.item [i-1].cells.item [4].value=2
						try myDGV.rows.item [i-1].cells.item [5].value = thisAnimSet.saveSkipFrames[i] catch myDGV.rows.item [i-1].cells.item [5].value=0
						try myDGV.rows.item [i-1].cells.item [8].value = thisAnimSet.saveStitchFinals[i] catch myDGV.rows.item [i-1].cells.item [8].value=false
						try(
						myDGV.rows.item[i-1].Cells.Item[6].value	= 	((thisAnimSet.saveEndFrames[rowIdx+1]-thisAnimSet.saveStartFrames[rowIdx+1])*((1.0/frameRate) as float)) as string
						myDGV.rows.item[i-1].Cells.Item[7].value	= ((thisAnimSet.saveEndFrames[rowIdx+1]-thisAnimSet.saveStartFrames[rowIdx+1])/(thisAnimSet.saveSkipFrames[rowIdx+1]+1)) as string)
						catch()
						
					)
					
				) 
				
				
fn onKeyPress s e =
(
format "key press: %\n" (bit.charasint e.keyChar)
)
fn onKeyDown s e =
(
format "key down: %\n" (e.keyCode.ToString())
)
on myDGV EditingControlShowing s e do -- When the cell is being edited
(
--hook.AssignHandle (dotnetobject "IntPtr" e.Control.Handle)
--dotNet.removeAllEventHandlers e.Control
dotNet.addEventHandler e.Control "KeyPress" onKeyPress
dotNet.addEventHandler e.Control "KeyDown" onKeyDown
)
on myDGV CellEndEdit s e do
(
format "cell end edit... : %\n" s.rows.item[e.RowIndex].cells.item[e.ColumnIndex].value
	if e.ColumnIndex==0 then thisAnimSet.saveActiveStates[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [0].value
	if e.ColumnIndex==1 then thisAnimSet.saveAnimClipNames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [1].value
	--if e.ColumnIndex==2 then thisAnimSet.saveAnimClipNames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [2].value
	if e.ColumnIndex==3 then (
		oldValue=thisAnimSet.saveStartFrames[e.RowIndex+1]
		try thisAnimSet.saveStartFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [3].value as integer catch(
			thisAnimSet.saveStartFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [3].value=oldValue
		)
		
	)
	if e.ColumnIndex==4 then (
		oldValue=thisAnimSet.saveEndFrames[e.RowIndex+1]
		try thisAnimSet.saveEndFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [4].value as integer catch(
			thisAnimSet.saveEndFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [4].value=oldValue
		)
	)
	if e.ColumnIndex==5 then (
		oldValue=thisAnimSet.saveEndFrames[e.RowIndex+1]
		try thisAnimSet.saveSkipFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [5].value as integer catch(
			thisAnimSet.saveSkipFrames[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [5].value=oldValue
		)
	)
	if e.ColumnIndex==8 then thisAnimSet.saveStitchFinals[e.RowIndex+1]=myDGV.rows.item [e.RowIndex].cells.item [8].value
)
				on btn_addAnimClip pressed do (
					myDGV.rows.add ()
					i=myDGV.rows.count
					append thisAnimSet.saveActiveStates true
					append thisAnimSet.saveAnimClipNames "new Anim ClipName"					
					append thisAnimSet.saveAnimSourceIDs ""
					try thisVal=(myDGV.rows.item [i-2].cells.item [3].value+100) catch thisVal=0
					append thisAnimSet.saveStartFrames thisVal
					try thisVal=(myDGV.rows.item [i-2].cells.item [4].value+100) catch thisVal=100
					append thisAnimSet.saveEndFrames thisVal
					append thisAnimSet.saveSkipFrames 0
					append thisAnimSet.saveStitchFinals false
					try myDGV.rows.item [i-1].cells.item [0].value = thisAnimSet.saveActiveStates[i] catch myDGV.rows.item [i-1].cells.item [0].value=false
					try myDGV.rows.item [i-1].cells.item [1].value = thisAnimSet.saveAnimClipNames [i] catch myDGV.rows.item [i-1].cells.item [1].value=""
					try myDGV.rows.item [i-1].cells.item [2].value = thisAnimSet.saveAnimSourceIDs[i] catch myDGV.rows.item [i-1].cells.item [2].value=""
					try myDGV.rows.item [i-1].cells.item [3].value = thisAnimSet.saveStartFrames[i] catch myDGV.rows.item [i-1].cells.item [3].value=1
					try myDGV.rows.item [i-1].cells.item [4].value = thisAnimSet.saveEndFrames[i] catch myDGV.rows.item [i-1].cells.item [4].value=2
					try myDGV.rows.item [i-1].cells.item [5].value = thisAnimSet.saveSkipFrames[i] catch myDGV.rows.item [i-1].cells.item [5].value=0
					try myDGV.rows.item [i-1].cells.item [8].value = thisAnimSet.saveStitchFinals[i] catch myDGV.rows.item [i-1].cells.item [8].value=false

						
				)
				on btn_del pressed do(print "del Animator")
				on btn_close pressed do(
					destroyDialog animClipManager_roll
				)

			)
			colorMax=DOTNETtoMXSRGB ((dotNetClass "System.Drawing.Color").LightGray)
			
			try destroyDialog animClipManager_roll catch ()
			createDialog animClipManager_roll --modal:true --bgcolor:colorMax

		
	)