plugin modifier AWDShadowMethod
name:"AWDShadowMethod"
category:"AWDModifier"
classID:#(0x201f7e8f, 0x2bd51f77)
extends:EmptyModifier replaceUI:false version:1
(
		
	local availableModsDisplayNames=#()
	local availableModsList=#()	
	local availableModsNamesList=#()
	local thisObj
	local thisMod
	local originalIdx=1
	
	fn getAWDID = (
		newTime=getUniversalTime()
		newString="awd_"+newTime[1] as string+newTime[2] as string+newTime[3] as string+newTime[4] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+newTime[5] as string+newTime[6] as string+newTime[7] as string+newTime[8] as string
		newString=newString+random 0.0 10000.0 as string
		return newString
	)
	

	parameters main rollout:params
	(
		thisAWDID type:#string default:"noIDSet"
		
		lightType type:#integer default:0		
		shadowMapper type:#integer default:1 ui:ddl_shadowMapper
		shadowMethod type:#integer default:1 ui:ddl_shadowMethod
		
	)
	rollout params "AWD Shadow Method"
	(		
		dropDownList ddl_shadowMapper "Shadow-Mapper" items:#("Directional", "Near Directional") width:140 pos:[10,10]
		dropDownList ddl_shadowMethod "Shadow-Method" items:#("Skeleton", "Vertex") width:140 pos:[10,60]
		on params open do(
			if lightType==1 then(
				ddl_shadowMapper.items=#("Directional", "Cascade", "Near Directional")
			)
			else if lightType==2 then(
				ddl_shadowMapper.items=#("Cube Map")
			)
		)
		
	)
	on create do (
		thisObj=selection[1]
		if (superclassof thisObj as string)=="light" then(
			if (classof thisObj as string)=="Directionallight" then lightType=2
			else lightType=1
		)
		
		--curAnimSetMod=modPanel.GetCurrentObject()
		--delegate.name = uniquename "AWD Anim Set"
		if thisAWDID=="noIDSet" then(
			thisAWDID=getAWDID()
		)
	)
	on clone orig do (				
		thisAWDID=getAWDID()		
	)

)