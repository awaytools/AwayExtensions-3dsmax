plugin modifier AWDSkeleton
name:"AWDSkeleton"
category:"AWDModifier"
classID:#(0x302349c, 0x57cb7278)
extends:EmptyModifier replaceUI:false version:1
(	
	local AnimSetRollout		
	local AnimClonesRollout			
	
	fn cntobjects thisObj = (
		cnt=1
		for child in thisObj.children do(
			cnt+=cntobjects child
		)
		return cnt
	)	
	fn checkIfObjIsSkinned thisObj skinnedlist= (
		cnt=0
		for thisName in skinnedlist do(
			if thisObj.name==thisName then(
				cnt+=1
			)
		)
		for child in thisObj.children do(
			cnt+=checkIfObjIsSkinned child skinnedlist
		)
		return cnt
	)	
	fn getSkinningInfos thisMod thisObj= (
		global thisSkelet=thisMod
		global currentObj=thisObj
		newStringList=""
		newStringListObjs=""
		maxNumUsedjoint=0
		global newSkinnedObjs=#()
		global newSkinnedObjsInfos=#()
		global newSkinnedObjNames=#()
		newSKelInfos=#()
		cntSkinnedMeshes=0
		global skelInfo_rollout
		try destroyDialog skelInfo_rollout catch()
		rollout rlProgBar "Get AWDSkeleton Infos" width:200
		(
			progressBar pbProgress "" pos:[10,22] width:180 height:15 value:0 color:[0,0,0]		 
		)
		createDialog rlProgBar
		objCnt=0
		maxJPV=0
		hasInvalidematrix=false
		allSkinnednames=#()
		allInsts = #()
		for i in objects do(
			objCnt+=1
			prog=((objCnt) as float /(objects.count) as float)*100
			rlProgBar.pbProgress.value = prog
			rlProgBar.pbProgress.color.r = (255-((255/100)*prog))
			rlProgBar.pbProgress.color.g = ((255/100)*prog)
			rlProgBar.pbProgress.color.b = 0
			for m in i.modifiers do(
				if classof  m as string=="Skin" then(
					if findItem allInsts i == 0 do(						
						numJoints=skinOps.GetNumberBones m
						select i
						modPanel.setCurrentObject  m
						skinnednames=#()						
						for j = 1 to numJoints do(
							testJoint= skinOps.GetBoneName  m j 1
							appendIfUnique skinnednames testJoint
							appendIfUnique allSkinnednames testJoint						
						)	
						cntSkinned = checkIfObjIsSkinned thisObj skinnednames
						if (cntSkinned>0) then (		
							this_jpv=0
							pointCntInt=selection[1].numverts
							for vertCnt=1 to pointCntInt do
								(
									BNumber = skinOps.getVertexWeightCount m vertCnt
									if BNumber>this_jpv then (
										if maxJPV<BNumber then maxJPV = BNumber
										this_jpv=BNumber
									)
								)	
							thisInstances = #()
							matrixOK=false
							if (InstanceMgr.GetInstances i &instances) > 1 do(
								if findItem allInsts i == 0 do join thisInstances instances
								for obj in instances do(
									objMatrix=at time (thisSkelet.neutralPose as integer) obj.transform
									skeletonMatrix=at time (thisSkelet.neutralPose as integer) thisObj.transform
									if objMatrix==skeletonMatrix then (
										matrixOK=true			
									)
										--print "yes"
										--print (isIdentity(objMatrix* (inverse skeletonMatrix) ))
								 
										--print "yes"
								)
							)
							join allInsts thisInstances
							if thisInstances.Count==0 then(
								append thisInstances i
							)
							print thisInstances.Count
							if not matrixOK then (
								objMatrix=(at time (thisSkelet.neutralPose as integer) i.transform) 
								skeletonMatrix=(at time (thisSkelet.neutralPose as integer) thisObj.transform)
								if objMatrix==skeletonMatrix then matrixOK=true		
							)
							if not matrixOK then hasInvalidematrix=true
							
							instanceCnt=1
							for oneInst in thisInstances do(
								append newSkinnedObjs oneInst
								append newSkinnedObjNames ("Instance: "+instanceCnt as string+"  -  name: '"+oneInst.name+"'  -  skinned joints: "+cntSkinned as string+"  -  max jpv: "+this_jpv as string )
								thisSkinInfos=#()
								append thisSkinInfos cntSkinned 
								if cntSkinned>maxNumUsedjoint then maxNumUsedjoint=cntSkinned
								append thisSkinInfos matrixOK
								append thisSkinInfos this_jpv
								append thisSkinInfos thisInstances
								append newSkinnedObjsInfos thisSkinInfos
								instanceCnt+=1
							)
							cntSkinnedMeshes+=1
						)
					)
					select thisObj
				)
			)
		)
		dialogPosition=[0,0];
		global skelInfo_rollout
		try (
			dialogPosition=(GetDialogPos skelInfo_rollout)
			destroyDialog skelInfo_rollout
			) 
		catch (
			hasDialog=false
		)
		try destroyDialog rlProgBar catch()
		newStringList="Skeleton is used by "+(cntSkinnedMeshes as string)+" skin-modifier\n\n"
		newStringList+=newStringListObjs
		try destroyDialog skelInfo_rollout catch()
		rollout skelInfo_rollout "Skeleton Infos" width:114
		(
				button btn_refresh "Refresh" width:80 height:30 pos:[10,10]
				button btn_openSkelInModPanel "Select Skeleton in mod-panel" width:160 height:30 pos:[250,10]
				label lbl_header_used "The AWD Skeleton is used by - skinned meshes" width:400 pos:[10,50]
				label lbl_used "joints used: " width:200 pos:[10,70]
				label lbl_maxJPV "Max joints per vert:"  width:200 pos:[200,70]
				label lbl_maxskinnedJoints "Joints used by skins:" width:200 width:200 pos:[10,90]
				label lbl_maxUsed "Max joints used by one skin:" pos:[200,90]
				MultiListBox  targetAnimClipsList "Used by skinned Meshes:" items:#() height:10  width:400 pos:[10,120]
				button btn_openInModPanel "Select in mod-panel" width:400 pos:[10,285]
				label lbl_used_skinned "joints used: " width:200 pos:[10,320]
				label lbl_maxJPV_skinned "max joints per vert: " width:200 pos:[200, 320]
					--label lbl_maxJPV_skinned "max joints per vert: " align:"left" width:100 pos:[10,10]
				
				label lbl_maxJPV_isIdentityMatrix "Attention!: At Neutral-Pose, one instance of each skinned mesh" width:400 pos:[60,350]
				label lbl_maxJPV_isIdentityMatrix3 "must be at the same global transform as the Skeleton-Root !!!" width:400 pos:[63,370]
				button btn_fixBindPose "Set transform of selected to the skeleton-transform + activate 'Skin Pose Mode'" width:400 height:30 pos:[10,390]
				on btn_openInModPanel pressed do(
					max modify mode 
					for i in targetAnimClipsList.selection do (
						modPanel.SetCurrentObject newSkinnedObjs[i]
					)
				)
				on btn_refresh pressed do(					
					thisSkelet.getSkinningInfos thisSkelet currentObj
				)
				
				on btn_fixBindPose pressed do(
					for i in targetAnimClipsList.selection do (
						thisObject=newSkinnedObjs[i]
						thisObject.skinPosEnabled  = false
						thisObject.skinRotEnabled   = false
						thisObject.skinScaleEnabled   = false
						thisObject.skinPoseMode =true
						thisObject.transform = at time (thisSkelet.neutralPose as integer) currentObj.transform
						newSkinnedObjsInfos[i][2]=true	
						--lbl_maxJPV_isIdentityMatrix.visible=false
						--lbl_maxJPV_isIdentityMatrix3.visible=false
					)
					test=sliderTime
					sliderTime=test+10
					sliderTime=test
				)
				on btn_openSkelInModPanel pressed do(
					max modify mode 
					modPanel.SetCurrentObject thisSkelet
					
				)
				
				on targetAnimClipsList selected thisSelectIdx do(
					maxJoints=0
					maxJPV2=0
					for i in targetAnimClipsList.selection do (
						if maxJoints<newSkinnedObjsInfos[i][1] then maxJoints=	newSkinnedObjsInfos[i][1]
						if maxJPV2<newSkinnedObjsInfos[i][3] then maxJPV2=	newSkinnedObjsInfos[i][3]
					)
					lbl_used_skinned.text= "Joints used by selected: "+maxJoints as string
					lbl_maxJPV_skinned.text= "Max joints per vert: "+maxJPV2 as string
										
				)
				on skelInfo_rollout open do(
					maxJoints=0
					maxJPV2=0
					for i in targetAnimClipsList.selection do (
						if maxJoints<newSkinnedObjsInfos[i][1] then maxJoints=	newSkinnedObjsInfos[i][1]
						if maxJPV2<newSkinnedObjsInfos[i][3] then maxJPV2=	newSkinnedObjsInfos[i][3]
					)
					lbl_used_skinned.text= "Joints used by selected: "+maxJoints as string
					lbl_maxJPV_skinned.text= "Max joints per vert: "+maxJPV2 as string
				)
		)
		createDialog skelInfo_rollout  width:420 height:430 style:#(#style_titlebar, #style_border, #style_sysmenu, #style_minimizebox)
		if dialogPosition!=[0,0] then SetDialogPos skelInfo_rollout dialogPosition					
		skelInfo_rollout.lbl_maxJPV.text= "Max joints per vert: "+(maxJPV as string)
		skelInfo_rollout.lbl_used.text= "Num joints: "+(thisMod.jointCnt as string)
		skelInfo_rollout.targetAnimClipsList.items= newSkinnedObjNames
		--skelInfo_rollout.lbl_jointScale.visible=false
		skelInfo_rollout.lbl_header_used.text="AWDSkeleton is used by "+ newSkinnedObjNames.Count as string +" skinned meshes" 
		skelInfo_rollout.lbl_maxUsed.text= "Max joints used by one skin: "+maxNumUsedjoint as string
		skelInfo_rollout.lbl_maxskinnedJoints.text= "Joints used by skins: "+allSkinnednames.Count as string
		skelInfo_rollout.lbl_maxJPV_isIdentityMatrix.visible=false
		skelInfo_rollout.lbl_maxJPV_isIdentityMatrix3.visible=false
		skelInfo_rollout.btn_fixBindPose.visible=false
		if (hasInvalidematrix) then(
			skelInfo_rollout.lbl_maxJPV_isIdentityMatrix.visible=true
			skelInfo_rollout.lbl_maxJPV_isIdentityMatrix3.visible=true
			skelInfo_rollout.btn_fixBindPose.visible=true
		)
		--messageBox(newStringList)
	)	
	
	fn createBonesForObjects originalBone parentBone = (
			newBone = Bone()
			newBone.transform = originalBone.transform
			newBone.parent = parentBone
			for child in originalBone.children do(
				createBonesForObjects child newBone
			)		
	)
	
	
	fn deleteKeysByRange con deleteRange =
	(
		for x in con.keys.count to 1 by -1 do	(
			if (con.keys[x].time >= deleteRange.start) and (con.keys[x].time <= deleteRange.end) do deleteKey con x
		)
	)

	fn deleteKeysByRangeObject obj thisInterval= (
		--mPointCon = obj.baseObject[#Master_Point_Controller]
		--if mPointCon != undefined do deleteKeysByRange mPointCon.controller (interval 0f 50f)
		deleteKeysByRange obj.position.controller thisInterval
		deleteKeysByRange obj.rotation.controller thisInterval
		deleteKeysByRange obj.scale.controller thisInterval
		for child in obj.children do deleteKeysByRangeObject child thisInterval		
	)
	fn CollectAllTargetObjs obj thisList = (
		append thisList obj
		for child in obj.children do CollectAllTargetObjs child thisList
	)
		
	fn bakeToSelectedClone thisSkelet = (
		animName=0
		animID=0
		animStart=0
		animEnd=0
		animSkip=0
		if thisSkelet.saveSelectedAnimClip>0 then (
			if thisSkelet.saveSelectedAnimClip<=thisSkelet.saveAnimClipNames.count then (
				animName=thisSkelet.saveAnimClipNames[thisSkelet.saveSelectedAnimClip]
				animID=thisSkelet.saveAnimSourceIDs[thisSkelet.saveSelectedAnimClip]
				animStart=thisSkelet.saveStartFrames[thisSkelet.saveSelectedAnimClip]
				animEnd=thisSkelet.saveEndFrames[thisSkelet.saveSelectedAnimClip]
				animSkip=thisSkelet.saveSkipFrames[thisSkelet.saveSelectedAnimClip]
				animStitchFinal=thisSkelet.saveStitchFinals[thisSkelet.saveSelectedAnimClip]
				animLoop=thisSkeleton.saveLoops[thisSkeleton.saveSelectedAnimClip]
				animTransform=thisSkeleton.saveTransform[thisSkeleton.saveSelectedAnimClip]
			)
		)
		if animStart==animEnd then (
			messageBox("No valid animationclip is selected")
			return()
		)
			
		rootObj=selection[1]
		targetRootObj=undefined
		targetModifier=undefined
		create=true
		if thisSkelet.selected_clone>0 then (
			if thisSkelet.selected_clone<=thisSkelet.savedClones.Count then (
				create=false
				targetModifier=thisSkelet.savedClones[thisSkelet.selected_clone]
				-- we have the correct modifierm but need to find the object that its applied to:
				--easy way: select the modifier, and check wich object is selected (if multiple are selected, take the first in the list)
				modPanel.SetCurrentObject targetModifier
				targetRootObj=selection[1]
				targetStartFrame=targetModifier.createClip targetModifier animName animID animStart animEnd animSkip animStitchFinal animLoop animTransform
				thisInterval = (interval targetStartFrame (targetStartFrame+(animEnd-animStart)))
				deleteKeysByRangeObject targetRootObj thisInterval
			)
		)
		if create then (
			targetRootObj = Bone()
			targetRootObj.transform = rootObj.transform
			global AWDSkeletonClone
			targetModifier = AWDSkeletonClone()
			addModifier targetRootObj targetModifier
			for child in rootObj.children do(
				createBonesForObjects child targetRootObj
			)				
			targetStartFrame=targetModifier.createClip targetModifier animName animID animStart animEnd animSkip animStitchFinal
			append thisSkelet.savedClones targetModifier
			targetModifier.target_ID=thisSkelet.thisAWDID
			targetModifier.skeletonRollout.txt_TargetSkel.text=targetModifier.name
		)
		i=animStart
		timeDifference=targetStartFrame-animStart
		didTheEnd=false
		allTargetObjects=#()
		CollectAllTargetObjs targetRootObj allTargetObjects
		allSourceObjTransforms=#()
		CollectAllTargetObjs rootObj allSourceObjTransforms
		rollout rlProgBar "Bar" width:114
		(
			progressBar pbProgress "" pos:[4,22] width:106 height:15 value:0 color:[0,0,0]		 
		)
		createDialog rlProgBar
		animate on(
			while i<=animEnd do(
				prog=((i-animStart) as float /(animEnd-animStart) as float)*100
				rlProgBar.pbProgress.value = prog
				rlProgBar.pbProgress.color.r = (255-((255/100)*prog))
				rlProgBar.pbProgress.color.g = ((255/100)*prog)
				rlProgBar.pbProgress.color.b = 0
				objCnter=1
				targetTime=(i+timeDifference) as integer
				for obj in allTargetObjects do(	
					thisSource=allSourceObjTransforms[objCnter]
					at time (targetTime as integer) in coordsys world  obj.transform =  at time (i as integer) in coordsys world thisSource.transform
					addNewKey obj[3] (targetTime as integer)
					objCnter+=1				
				)					
				forcedEnd=false
				if i==animEnd then didTheEnd=true
					
				i+=1+animSkip
				
				if i>animEnd and not didTheEnd then i=animEnd--do the end
				
			)
		)
		deleteItem  thisSkelet.saveAnimClipNames thisSkelet.saveSelectedAnimClip			
		deleteItem  thisSkelet.saveAnimSourceIDs thisSkelet.saveSelectedAnimClip
		deleteItem  thisSkelet.saveStartFrames thisSkelet.saveSelectedAnimClip
		deleteItem  thisSkelet.saveEndFrames thisSkelet.saveSelectedAnimClip
		deleteItem  thisSkelet.saveSkipFrames thisSkelet.saveSelectedAnimClip
		deleteItem  thisSkelet.saveStitchFinals thisSkelet.saveSelectedAnimClip	
		deleteItem  thisSkeleton.saveLoops thisSkeleton.saveSelectedAnimClip
		deleteItem  thisSkeleton.saveTransform thisSkeleton.saveSelectedAnimClip	
		thisSkelet.saveSelectedAnimClip-=1
		thisInterval = (interval animStart animEnd)
		deleteKeysByRangeObject rootObj thisInterval
		modPanel.SetCurrentObject targetModifier
		try destroyDialog rlProgBar catch()
		
	)	
	fn chooseAnimationSet thisObj thisModifier = (		
			
		global animDisplayNames=#()
		global animSets=#()	
		global thisMesh=thisObj
		global thisMod=thisModifier
		global originalIdx=1
		foundAny=false
		for i in objects do(
			for m in i.modifiers do(
				if (classof m as string)=="AWDAnimationSet" then(
					if m.animType==1 then(
						foundAny=true	
						allReadyAssigned=false
						if m.sourceSkeletonID!="noIDSet" then (
							if oneSkelSourceID==thisModifier.thisAWDID then(
								allReadyAssigned=true
								exit
							)
						)
						if not allReadyAssigned then (
							appendThis= (appendIfUnique animSets m)
							if appendThis then append animDisplayNames ("'"+m.name+"'  object: "+i.name) 							
						)
					)
				)
			)
		)
		
		if animSets.count>0 then(
			rollout select_animSet_rollout "Select a AWDAnimationSet-Modifier"	(
				listbox base_name "AWDAnimationSet-Modifier available in scene"  height:16 items:animDisplayNames selection:originalIdx
				on base_name selected selected do (
					append thisMod.saveAnimSets animSets[base_name.selection]		
					animSets[base_name.selection].sourceSkeletonID=thisMod.thisAWDID
					modPanel.SetCurrentObject animSets[base_name.selection]
					thisMod.animSetsSelected=animSets.Count
					modPanel.SetCurrentObject thisMod
					DestroyDialog select_animSet_rollout
				)
			)
			--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
			createDialog select_animSet_rollout 300 240  modal:true
		)
		else	(
			if foundAny then	messagebox("The AWDSkeleton allready is assigned to all available AWDAnimationSets (Skeleton) in the scene.")
			else messagebox("No AWDAnimationSets-modifier with of type='Skeleton' could be found in the scene")
		)
	)
	
	fn chooseClone thisObj thisModifier = (		
			
		global animDisplayNames=#()
		global animSets=#()	
		global thisMesh=thisObj
		global thisMod=thisModifier
		global originalIdx=1
		
		foundAny=false
		for i in objects do(
			for m in i.modifiers do(
				if (classof m as string)=="AWDSkeletonClone" then(
					foundAny=true	
					allReadyAssigned=false
					if m.target_ID!="noIDSet" then (
						if m.target_ID==thisModifier.thisAWDID then(
							allReadyAssigned=true
						)
					)
					if not allReadyAssigned then (
						appendThis= (appendIfUnique animSets m)
						if appendThis then append animDisplayNames ("'"+m.name+"'  object: "+i.name) 							
					)
				)
			)
		)
		
		if animSets.count>0 then(
			rollout select_animSet_rollout "Select a AWDSkeletonClone-Modifier"	(
				listbox base_name "AWDSkeletonClone-Modifier available in scene"  height:16 items:animDisplayNames selection:originalIdx
				on base_name selected selected do (
					append thisMod.savedClones animSets[base_name.selection]		
					animSets[base_name.selection].target_ID=thisMod.thisAWDID
					modPanel.SetCurrentObject animSets[base_name.selection]
					thisMod.selected_clone=animSets.Count
					modPanel.SetCurrentObject thisMod
					thisMod.AnimClonesRollout.btn_selectClone.enabled=true
					thisMod.AnimClonesRollout.btn_clearClone.enabled=true
					DestroyDialog select_animSet_rollout
				)
			)
			--open as modal dialog, to prevent switching the user from changing the selected obj befor applying a new AWDAnimator
			createDialog select_animSet_rollout 300 240  modal:true
		)
		else	(
			if foundAny then	messagebox("All AWDSkeletonClone-modifier found in scene are allready assigned to this AWDSkeleton.")
			else messagebox("No AWDSkeletonClone-modifier could be found in the scene")
		)
	)	
	--skeletonHelper functions: move somewhere else ?
	fn checkParent thisMesh = (
		if thisMesh != undefined then(
			for m in thisMesh.modifiers do(
				if classof m as string == "AWDSkeleton" then messagebox("Found a AWDSkeleton-modifier one of the parents.")
			)
			checkParent thisMesh.parent			
		)
	)
	 
	fn constructHirarchyObjList thisMesh = (
		thisList=#(thisMesh)
		for child in thisMesh.children do(
			returnList=constructHirarchyObjList child 
			for this in returnList do(
				append thisList this
			)
		)
		return thisList
	)
	
	fn constructHirarchyList thisMesh = (
		thisList=#(thisMesh.children.count)
		for child in thisMesh.children do(
			for m in child.modifiers do(
				if classof m as string == "AWDSkeleton" then messagebox("Found a AWDSkeleton-modifier furter down the hirarchy.")
			)
			returnList=constructHirarchyList child 
			for this in returnList do(
				append thisList this
			)
		)
		return thisList
	)
		
	parameters skeletonParams rollout:skeletonRollout
	(
		thisAWDID type:#string default:"noIDSet" animatable:false
		jointPerVert type:#integer ui:SPN_jointPerVert default:2 animatable:false
		jointCnt type:#string default:"Joints: 0" animatable:false
		neutralPose type:#integer ui:SPN_neutralPose default:0 animatable:false
		skinnedJointCnt type:#string default:"SkinnedJoints: 0" animatable:false
		hirarchyString type:#string animatable:false
		simpleMode type:#boolean ui:CBX_SimpleMode default:true animatable:false
		simpleMode2 type:#boolean ui:CBX_SimpleMode2 default:true animatable:false
		
	)
	rollout skeletonRollout "AWD Skeleton" category:1
	(			
		label SPACER_1 "" height:5
		label lbl_jointCnt "" 
		label SPACER_2 "" height:5
		spinner SPN_jointPerVert "Joints per Vert" range:[1,10,1] type:#integer align:#right 
		spinner SPN_neutralPose "Neutral Pose" range:[-9999999,9999999,1] type:#integer align:#right 
		label SPACER_3 "" height:5
		button btn_recheck "Skeleton Infos" width:140
		checkBox CBX_SimpleMode "Auto Create Anim-Setup" width:140 enabled:true
		checkBox CBX_SimpleMode2 "Shared Animator" width:140 enabled:true
			
		on btn_recheck pressed do (		
			cntAll=cntobjects selection[1]	
			jointCnt="Joints: "+cntAll as string
			lbl_jointCnt.text=jointCnt
			
			curMod=modPanel.GetCurrentObject()
			getSkinningInfos curMod selection[1]
			modPanel.SetCurrentObject curMod
		)
		on CBX_SimpleMode changed newValue do (	
			removeRollout AnimSetRollout
			CBX_SimpleMode2.enabled=true
			if not newValue then (
				CBX_SimpleMode2.enabled=false
				addRollout AnimSetRollout
				addRollout AnimClonesRollout	
			)				
		)
		on skeletonRollout open do (		
			lbl_jointCnt.text=jointCnt
		)
	)	
	parameters AnimClipsparams rollout:AnimClipsRollout
	(
		
		saveSelectedAnimClip type:#integer default:1
		saveAnimClipNames type:#stringTab tabSize:0 tabSizeVariable:true
		saveAnimSourceIDs type:#stringTab tabSize:0 tabSizeVariable:true
		saveStartFrames type:#intTab tabSize:0 tabSizeVariable:true
		saveEndFrames type:#intTab tabSize:0 tabSizeVariable:true
		saveSkipFrames type:#intTab tabSize:0 tabSizeVariable:true
		saveStitchFinals type:#boolTab tabSize:0 tabSizeVariable:true
		saveLoops type:#boolTab tabSize:0 tabSizeVariable:true
		saveTransform type:#boolTab tabSize:0 tabSizeVariable:true		
		
	) 
	rollout AnimClipsRollout "Animation Clips" category:2	
	(				
		label lbl_listHeader " Name    Start       End"    width:140 pos:[8,10]
		listbox targetAnimClipsList "" items:#()   width:150 pos:[6,28]
		button btn_newFromRange "New Clip from Range" width:135 pos:[13,170]
		button btn_SetRange "Set Range from Clip" width:135 pos:[13,200]
		button btn_SetRangetoClip "Set Range to Clip" width:135 pos:[13,230]
		button btn_AWDClipManager "ClipManager" width:135 pos:[13,260]
		button btn_bakeToClone "Bake to clone" width:135 pos:[13,290]
		on btn_bakeToClone pressed do(
			curMod=modPanel.GetCurrentObject()
			bakeToSelectedClone curMod
		)
		on targetAnimClipsList selected curState do(
			print targetAnimClipsList.selection
		)
		
		on btn_AWDClipManager pressed do(
			curMod=modPanel.GetCurrentObject()
			global openAWDClipManager
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDAnimationClipManager.ms")
			saveSelectedAnimClip=targetAnimClipsList.selection
			openAWDClipManager curMod "Vertex" targetAnimClipsList.selection
		)
		on cbx_anim_only changed curState do(
			if curState then (
				SPN_jointPerVert.enabled=false
			)
			else (
				SPN_jointPerVert.enabled=true
			)
		)			
		on btn_newFromRange pressed do (
			curMod=modPanel.GetCurrentObject()
			firstframe=0
			lastframe=0
			valid=false
			irowCnt=saveAnimClipNames.count+1
			theObj = dotNetObject "MaxCustomControls.RenameInstanceDialog" ("AWDAnimationClip #"+irowCnt as string)
			theobj.text ="Add name for new AWDAnimationClip"
			DialogResult = theObj.Showmodal()
			isOK=dotnet.compareenums TheObj.DialogResult ((dotnetclass "System.Windows.Forms.DialogResult").OK)
			if isOK then (
				fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
				result = theobj.InstanceName
				try(
					append saveAnimClipNames result
					append saveAnimSourceIDs (getAWDID() as string)
					append saveStartFrames animationRange.start
					append saveEndFrames animationRange.end
					append saveSkipFrames 0
					append saveStitchFinals false
					append saveLoops false
					append saveTransform false
					valid=true
				)
				catch()
				if not valid then messageBox("Error - clip was not added")
				curMod=modPanel.GetCurrentObject()
				updateUI curMod targetAnimClipsList.selection
				targetAnimClipsList.selection=(targetAnimClipsList.items.count)
				saveSelectedAnimClip=targetAnimClipsList.selection
				global openAWDClipManager
				--fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDAnimationClipManager.ms")
				hasDialog=true
-- 				global awd_animation_rollout
-- 				try (
-- 					destroyDialog awd_animation_rollout
-- 				)
-- 				catch (hasDialog=false)
-- 				if hasDialog then openAWDClipManager curMod "Vertex" targetAnimClipsList.selection
			)
		)
		on btn_SetRange pressed do (
			curMod=modPanel.GetCurrentObject()
			firstframe=0
			lastframe=0
			valid=false
			try(
				firstframe=saveStartFrames[targetAnimClipsList.selection]
				lastframe=saveEndFrames[targetAnimClipsList.selection]
				valid=true
			)
			catch()
			if firstframe<lastframe and valid then animationRange = interval firstframe lastframe --Set range to frames 10 to 50
			else messageBox("Invalid time-range set for the selected clip")
			curMod=modPanel.GetCurrentObject()
			updateUI curMod (targetAnimClipsList.selection-1)
		)
		on btn_SetRangetoClip pressed do (
			valid=false
			try(
				saveStartFrames[targetAnimClipsList.selection]=animationRange.start
				saveEndFrames[targetAnimClipsList.selection]=animationRange.end
				valid=true
			)
			catch()
			if not valid then messageBox("Error - time range was not set for the clip")
			curMod=modPanel.GetCurrentObject()
			updateUI curMod (targetAnimClipsList.selection-1)
		)
		on AnimClipsRollout open do (	
			curMod=modPanel.GetCurrentObject()
			updateUI curMod (saveSelectedAnimClip-1)
		)
	)
	parameters AnimSetParams rollout:AnimSetRollout
	(
		
		animSetsSelected type:#integer default:0
		saveAnimSets type:#maxObjectTab tabSize:0 tabSizeVariable:true
	) 
	rollout AnimSetRollout "Assigned to Animation Sets" category:3		
	(		
		listBox targetAnimSetsList "" items:#()
		button btn_newAnimSet "new"   width:30  pos:[13,150]
		button btn_addAnimSet "add"   width:30  pos:[48,150]
		button btn_selectAnimSet "sel"  width:30 pos:[83,150]
		button btn_clearAnimSet "del"   width:30  pos:[118,150]
		on targetAnimSetsList selected curIndex do(
			animSetsSelected=curIndex
		)
		on btn_newAnimSet pressed do (
			thisMesh=selection[1]
			thisMod= modPanel.getCurrentObject()
			MyNewModifier = AWDAnimationSet()
			MyNewModifier.name = "AWD Anim Set"
			addmodifier thisMesh MyNewModifier
			MyNewModifier.sourceSkeletonID=thisMod.thisAWDID
			animSetsSelected=saveAnimSets.Count
			modPanel.setCurrentObject thisMod
			append saveAnimSets MyNewModifier
			append targetAnimSetsList.items MyNewModifier.name
			
		)
		on btn_addAnimSet pressed do(
			chooseAnimationSet selection[1] (modPanel.getCurrentObject())
		)
		--Remove currently selected item from the list:
		on btn_selectAnimSet pressed do
		(
			if targetAnimSetsList.items.count > 0 and targetAnimSetsList.selection > 0 do (				
				modPanel.setCurrentObject saveAnimSets[targetAnimSetsList.selection]
			)
		)
		--Remove currently selected item from the list:
		on btn_clearAnimSet pressed do
		(
			if targetAnimSetsList.items.count > 0 and targetAnimSetsList.selection > 0 do(
				--todo: delete this AWDSkeleton from the AWDAnimSet
				saveAnimSets = deleteItem saveAnimSets targetAnimSetsList.selection
				targetAnimSetsList.items = deleteItem targetAnimSetsList.items targetAnimSetsList.selection
				animSetsSelected=animSetsSelected-1
			)
 			if targetAnimSetsList.items.count > 0 then (
				if animSetsSelected>targetAnimSetsList.items.count then animSetsSelected=targetAnimSetsList.items.count
 				targetAnimSetsList.selection=animSetsSelected
			)
			
		)
		on AnimSetRollout open do (		
			if simpleMode then removeRollout AnimSetRollout
			else addRollout AnimSetRollout
			addRollout AnimClipsRollout
			--targetAnimClipsList.items=saveAnimClipNames
-- 			--check the targetAnimSet-list
			targetAnimSetsList.items=#()
			newAnimSets=#()
			for oneAnimSet in saveAnimSets do(
				if oneAnimSet.sourceSkeletonID!=undefined then(
 					if oneAnimSet.sourceSkeletonID==thisAWDID then(
 						for i in objects do (
 							hasfound=false
 							for m in i.modifiers do(
 								if m==oneAnimSet then (
 									hasfound=true
									appendThis = (appendIfUnique newAnimSets m)
									if appendThis then targetAnimSetsList.items = append targetAnimSetsList.items (oneAnimSet.name as string)
 									exit
 								)
 							)
 						if hasfound then exit
 						)
					)
				)
			)			
 			--if targetAnimSetsList.items.count >=animSetsSelected then 
 			--(
 			--	targetAnimSetsList.selection=animSetsSelected
 			--)
			--if targetAnimSetsList.items.count < animSetsSelected then animSetsSelected=targetAnimSetsList.items.count
			--if targetAnimSetsList.items.count > 0 then targetAnimSetsList.selection=animSetsSelected
							
			btn_selectAnimSet.enabled=false
			btn_clearAnimSet.enabled=false
			if targetAnimSetsList.items.count > 0 and targetAnimSetsList.selection > 0 do(
			--if AnimationSetID=="noIDSet" then(
				--animSetName.text=""
				btn_selectAnimSet.enabled=true
				btn_clearAnimSet.enabled=true
			--)
			)
		)
		
	)
	parameters AnimClonesParams rollout:AnimClonesRollout
	(
		
		selected_clone type:#integer default:0
		savedClones type:#maxObjectTab tabSize:0 tabSizeVariable:true
	) 
	rollout AnimClonesRollout "Skeleton Clones" category:4	
	(		
		listBox targetClonesList "" items:#()
		button btn_addClone "add"   width:40  pos:[13,150]
		button btn_selectClone "sel"  width:40 pos:[61,150]
		button btn_clearClone "del"   width:40  pos:[108,150]
		on targetClonesList selected curIndex do(
			selected_clone=curIndex
		)
		on btn_newClone pressed do (
			thisMesh=selection[1]
			thisMod= modPanel.getCurrentObject()
			global AWDSkeletonClone
			MyNewModifier = AWDSkeletonClone()
			MyNewModifier.name = "AWD Skeleton Clone"
			addmodifier thisMesh MyNewModifier
			MyNewModifier.target_ID=thisMod.thisAWDID
			append savedClones MyNewModifier
			selected_clone=savedClones.Count
			btn_selectClone.enabled=true
			btn_clearClone.enabled=true
			modPanel.setCurrentObject thisMod
			
		)
		on btn_addClone pressed do(
			chooseClone selection[1] (modPanel.getCurrentObject())
		)
		--Remove currently selected item from the list:
		on btn_selectClone pressed do
		(
			print savedClones[targetClonesList.selection].name 
			if targetClonesList.items.count > 0 and targetClonesList.selection > 0 do (				
				modPanel.setCurrentObject savedClones[targetClonesList.selection]
			)
		)
		--Remove currently selected item from the list:
		on btn_clearClone pressed do
		(
			if targetClonesList.items.count > 0 and targetClonesList.selection > 0 do(
				savedClones[targetClonesList.selection].target_ID="noIDSet"
				savedClones = deleteItem savedClones targetClonesList.selection
				targetClonesList.items = deleteItem targetClonesList.items targetClonesList.selection
				selected_clone=selected_clone-1
			)
 			if targetClonesList.items.count > 0 then (
				if selected_clone>targetClonesList.items.count then selected_clone=targetClonesList.items.count
 				targetClonesList.selection=selected_clone
			)
			if targetClonesList.items.count == 0 then(
				btn_selectClone.enabled=false
				btn_clearClone.enabled=false
			)
			
		)
		on AnimClonesRollout open do (		
			--targetAnimClipsList.items=saveAnimClipNames
			--check the targetAnimSet-list
			targetClonesList.items=#()
			newClones=#()
			for oneClone in savedClones do(
				if oneClone.target_ID==thisAWDID then(
 					for i in objects do (
 						hasfound=false
 						for m in i.modifiers do(
 							if m==oneClone then (
 								hasfound=true
								appendThis = (appendIfUnique newClones m)
								if appendThis then targetClonesList.items = append targetClonesList.items (oneClone.name as string)
 								exit
 							)
 						)
						if hasfound then exit
					)
				)
			)		
			savedClones=newClones			
 			if targetClonesList.items.count >=selected_clone then 
 			(
 				targetClonesList.selection=selected_clone
 			)
			if targetClonesList.items.count < selected_clone then selected_clone=targetClonesList.items.count
			if targetClonesList.items.count > 0 then targetClonesList.selection=selected_clone
							
			btn_selectClone.enabled=true
			btn_clearClone.enabled=true
			if targetClonesList.items.count == 0 then(
				btn_selectClone.enabled=false
				btn_clearClone.enabled=false
			)
		)
		
	)
	
	on create do (
		if thisAWDID=="noIDSet" then(				
			fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
			thisAWDID=getAWDID()
		)
		--name = uniquename "AWDSkeleton"
		--checkParent selection[1].parent
		--hirarchyList=constructHirarchyList selection[1]
		--firstList=constructHirarchyObjList selection[1]
		--secondList=constructHirarchyObjList objects[1]
		
		--global retargetingSources=#(firstList)
		--global retargetingTargets=#(secondList)
		--if hirarchyList.count<2 then 	print "toDo the skeleton contains no joints. its not valid. delete!"
		--jointCnt = "Joints: "+hirarchyList.count as string
		--if thisAWDID=="noIDSet" then thisAWDID=getAWDID()
		--append savedInfos ("no information available"+thisAWDID as string)
	)
	on clone orig do thisAWDID=getAWDID()

)