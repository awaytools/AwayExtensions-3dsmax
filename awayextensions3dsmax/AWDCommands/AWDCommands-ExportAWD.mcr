macroScript ExportAWD
	category:"AWDCommands"
	toolTip:""
(
	onlyFileName=filterString maxFileName "." 
	if onlyFileName.Count==0 then onlyFileName=#("NewAWDFile")
		
	newAWDFileName=maxFilePath + onlyFileName[1] + ".awd"
	newFileName=getSaveFileName caption:"Save as AWD-File" fileName:newAWDFileName
	if newFileName!=undefined then(
		exportFile newFileName -- [ #noPrompt ] [ selectedOnly:<boolean> ] [ using:<maxclass> ] 
	)
)
