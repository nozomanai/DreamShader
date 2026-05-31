#include "DreamShaderCompileService.h"

namespace UE::DreamShader::Compiler
{
	FDreamShaderCompileResult FDreamShaderCompileService::CompileAssets(const FString& SourceFilePath, const bool bForce)
	{
		FDreamShaderCompileRequest Request;
		Request.SourceFilePath = SourceFilePath;
		Request.bForce = bForce;
		return Compiler.CompileAssets(Request);
	}

	FDreamShaderCompileResult FDreamShaderCompileService::CompileMaterial(const FString& SourceFilePath, const bool bForce)
	{
		FDreamShaderCompileRequest Request;
		Request.SourceFilePath = SourceFilePath;
		Request.bForce = bForce;
		return Compiler.CompileMaterial(Request);
	}
}
