macroScript addAWDMaterialSettings
	category:"AWDCommands"
	toolTip:"Add AWDMaterialSettings to selected Material"
(
		
	(		
		if not MatEditor.isOpen() then MatEditor.open() 
		mtl=undefined
		--if material mode is "Compact-Mode"
		if MatEditor.mode ==#basic then (
			mtl = meditMaterials[activeMeditSlot] 
		)
		--if material mode is "Slate-Mode"
		else if MatEditor.mode ==#advanced then (
			mtl=sme.GetMtlInParamEditor()		
		)
		--if neither "Compact-Mode" nor "Slate-Mode" (should not happen)
		else (
			messagebox("ERROR:\nCould not read a valid MatEditor Mode\n(Neither 'Compact' nor 'slate')\n")
			return()
		)		
		
		foundMats=#()
		if mtl!=undefined then (
			if classof mtl == Standardmaterial then append foundMats mtl
			else if classof mtl == Multimaterial then (
				for mat in mtl.materialIdList do	(
					submat = mtl[mat]
					if submat != undefined then (
						if classof submat == Standardmaterial then 	(
							append foundMats submat
						)
					)
				)
				if foundMats.Count==0 then (
					messagebox("The active Material in the Material-Editor is a MultiMaterial, but does not contain any Standart-Materials that could have AWDMaterialSettings assigned\n")
					return()
				)
			)
			else (
				messagebox("The active Material in the Material-Editor must be of type 'Standart' or 'MultiMaterial'\n")
				return()
			)
		)
		else (
			answer=queryBox ("No material is selected in the Material-Editor.\nApply AWDMaterialSettings to all material(s) on selected SceneObjects?")
			if not answer then return()
			matObjects = selection
			if matObjects.count==0 then (
				answerAll=queryBox ("No SceneObjects selected.\nApply AWDMaterialSettings to all material(s) on all SceneObjects?")
				if not answerAll then return()
				matObjects = objects
				if matObjects.count==0 then (
					messagebox("No SceneObjects found'\n")
					return()
				)					
			)			
			for oneMesh in matObjects do (
				if oneMesh.material!=undefined then(
					if classof oneMesh.material == Standardmaterial then append foundMats oneMesh.material
					else if classof oneMesh.material == Multimaterial then (
						for mat in oneMesh.material.materialIdList do	(
							thisSubmat = oneMesh.material[mat]
							if thisSubmat != undefined then (
								if classof thisSubmat == Standardmaterial then 	(
									append foundMats thisSubmat
								)
							)
						)
					)
				)
			)
		)
		failedToCreate=0
		if foundMats.count==0 then (	
			messagebox("No Materials found that could have AWDMaterialSettings assigned'\n")
			return()
		)
		else	for oneMat in foundMats do	(
			if classof oneMat == Standardmaterial then (
				createdSettings = addAWDMaterialSettings oneMat		
				if not createdSettings then failedToCreate+=1
				--showShadingrollups oneMat
				fileIn (getDir #maxRoot + "\\plugins\\AwayExtensions3dsMax\\AWDHelperScripts\\AWDGeneralHelpers.ms")--reload this file here, so the function is available after restoring the 3dsmax-scene
				oneMat.thisAWDID=getAWDID()
			)
			else failedToCreate+=1
		)
		if failedToCreate>0 then (
			if failedToCreate==foundMats.count then messagebox("Did not create AWDMaterialSettings, because all selected materials allready have AWDMaterialSettings assigned\n")
			else messagebox("Did not create AWDMaterialSettings for all selected materials, because some of them allready have AWDMaterialSettings assigned\n")
		)
			
	)
)
