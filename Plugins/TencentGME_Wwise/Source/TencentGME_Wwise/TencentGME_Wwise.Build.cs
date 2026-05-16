using System;
using UnrealBuildTool;
using System.IO;

public class TencentGME_Wwise : ModuleRules
{
	private static string GetConfiguration(ReadOnlyTargetRules Target)
	{
		switch (Target.Configuration)
		{
			case UnrealTargetConfiguration.Debug:
				// change bDebugBuildsActuallyUseDebugCRT to true in BuildConfiguration.cs to actually link debug binaries
				return Target.bDebugBuildsActuallyUseDebugCRT ? "Debug" : "Profile";

			case UnrealTargetConfiguration.Development:
			case UnrealTargetConfiguration.Test:
			case UnrealTargetConfiguration.DebugGame:
				return "Profile";

			default:
				return "Release";
		}
	}

	private static void CheckCompilerVersion(ref string Version, WindowsCompiler Compiler, string LongVersionName, string ShortVersionName)
	{
		try
		{
			if (Compiler == (WindowsCompiler)Enum.Parse(typeof(WindowsCompiler), LongVersionName))
				Version = ShortVersionName;
		}
		catch
		{
		}
	}

	private string GetVisualStudioVersion()
	{
		string VSVersion = "vc160";
		var Compiler = Target.WindowsPlatform.Compiler;
		CheckCompilerVersion(ref VSVersion, Compiler, "VisualStudio2022", "vc170");
		CheckCompilerVersion(ref VSVersion, Compiler, "VisualStudio2019", "vc160");
		CheckCompilerVersion(ref VSVersion, Compiler, "VisualStudio2017", "vc150");
		return VSVersion;
	}

	private void AddArchives(string GMESDKDir, string platformName, string GMEConfigurationDir, string gmeLibraryName = "libGMESDK.a")
	{
		string sdkDir = Path.Combine(GMESDKDir, platformName, GMEConfigurationDir, "lib");
		PublicAdditionalLibraries.Add(Path.Combine(sdkDir, "libTencentGMEPlugin.a"));
		PublicAdditionalLibraries.Add(Path.Combine(sdkDir, gmeLibraryName));
	}

	private void AddAppleArchives(string GMESDKDir, string platformName, string GMEConfigurationDir)
	{
		AddArchives(GMESDKDir, platformName, GMEConfigurationDir);

		PublicAdditionalLibraries.AddRange(
			new[] {
				"iconv",
				"resolv"
			}
		);

		PublicFrameworks.AddRange(
			new[] {
				"AVFoundation",
				"AudioToolbox",
				"CoreAudio"
			}
		);
	}

	private void AddStaticAndDynamicLibraries(string GMESDKDir, string platformPrefix, string GMEConfigurationDir, string visualStudioVersion = "")
	{
		if (string.IsNullOrEmpty(visualStudioVersion))
			visualStudioVersion = GetVisualStudioVersion();

		string sdkDir = Path.Combine(GMESDKDir, platformPrefix + visualStudioVersion, GMEConfigurationDir);
		PublicAdditionalLibraries.Add(Path.Combine(sdkDir, "bin", "TencentGME.lib"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/gmesdk.dll", Path.Combine(sdkDir, "bin", "gmesdk.dll"));
		RuntimeDependencies.Add("$(BinaryOutputDir)/TencentGME.dll", Path.Combine(sdkDir, "bin", "TencentGME.dll"));
		PrivateDefinitions.Add("TENCENTGME_WWISE_DYNAMIC");
	}

	public TencentGME_Wwise(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new[] {
				"Core",
				"CoreUObject",
				"Engine",
				"AkAudio",
				"Projects"
			}
		);

		string GMESDKDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty"));
		PublicIncludePaths.Add(Path.Combine(GMESDKDir, "include"));

		string GMEConfiguration = GetConfiguration(Target);
		Console.WriteLine("LogTencentGME: Config: {0}, Platform: {1}", GMEConfiguration, Target.Platform);

		switch (Target.Platform.ToString())
		{
			case "Android":
				foreach (string AndroidArch in new[] { "arm64-v8a", "armeabi-v7a", "x86", "x86_64" })
				{
					string libPathToInclude = Path.Combine(GMESDKDir, "Android", AndroidArch, GMEConfiguration, "bin", "libTencentGME.so");
					PublicAdditionalLibraries.Add(libPathToInclude);
				}

				PrivateDefinitions.Add("TENCENTGME_WWISE_DYNAMIC");
				PrivateDependencyModuleNames.Add("Launch");

				string aplPath = Path.Combine(Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath), "GMESDK_APL.xml");
				AdditionalPropertiesForReceipt.Add("AndroidPlugin", aplPath);
				break;

			case "IOS":
				AddAppleArchives(GMESDKDir, "iOS_Xcode1400", GMEConfiguration + "-iphoneos");
				break;

			case "Mac":
				AddAppleArchives(GMESDKDir, "Mac_Xcode1400", GMEConfiguration);
				break;

			case "PS4":
				var ExpectedPs4SdkVersion = Environment.ExpandEnvironmentVariables("%SCE_ORBIS_SDK_DIR%");
				var Ps4PlatformFolder = "PS4_SDK" + System.IO.Path.GetFileName(ExpectedPs4SdkVersion);
				AddArchives(GMESDKDir, Ps4PlatformFolder, GMEConfiguration, "libGMESDK.a");
				break;

			case "PS5":
				var ExpectedPs5SdkVersion = Environment.ExpandEnvironmentVariables("%SCE_PROSPERO_SDK_DIR%");
				ExpectedPs5SdkVersion = System.IO.Path.GetFileName(ExpectedPs5SdkVersion);
				if (ExpectedPs5SdkVersion.Contains("9.000"))
				{
					ExpectedPs5SdkVersion += "_ABIv1";
				}
				if (ExpectedPs5SdkVersion.Contains("10.000"))
				{
					ExpectedPs5SdkVersion += "_ABIv1";
				}
				var Ps5PlatformFolder = "PS5_SDK" + ExpectedPs5SdkVersion;
				System.Console.WriteLine($"LogTencentGME: Using {Ps5PlatformFolder}");
				AddArchives(GMESDKDir, Ps5PlatformFolder, GMEConfiguration, "libGMESDK.a");
				break;

			case "Win32":
				AddStaticAndDynamicLibraries(GMESDKDir, "Win32_", GMEConfiguration);
				break;

			case "Win64":
				AddStaticAndDynamicLibraries(GMESDKDir, "x64_", GMEConfiguration);
				break;

			case "XB1":
			case "XboxOne":
			case "XboxOneGDK":
				AddStaticAndDynamicLibraries(GMESDKDir, "XboxOneGC_", GMEConfiguration);
				break;

			case "XSX":
				AddStaticAndDynamicLibraries(GMESDKDir, "XboxSeriesX_", GMEConfiguration);
				break;

			case "Switch":
				AddArchives(GMESDKDir, "NX64_SDK16", GMEConfiguration);
				break;

			default:
				throw new System.Exception(string.Format("Platform <{0}> not supported by TencentGME_Wwise.", Target.Platform));
		}
	}
}
