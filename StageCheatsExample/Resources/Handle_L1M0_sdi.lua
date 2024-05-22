local Path = GetPath()
local GamePath = GetGamePath(Path)

local File = ReadFile(GamePath)
local MFK = MFKLexer.Lexer:Parse(File)

for i=1,#MFK.Functions do
	local Func = MFK.Functions[i]
	if Func.Name:lower() == "addstage" then
		MFK:InsertFunction(i + 1, "SetStageCheatEnabled", 1 << 9)
		break
	end
end

MFK:Output(true)