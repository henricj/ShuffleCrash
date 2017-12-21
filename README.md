# ShuffleCrash
Codegen or ICE with MSVC 19.12.25831

The daily MSVC build have already fixed this.  (Tested with "Microsoft
(R) C/C++ Optimizing Compiler Version 19.13.26020 for x64".)

Here's the actual bug report:
https://developercommunity.visualstudio.com/content/problem/172370/bad-codegen-with-x64release-build.html

Running ShuffleCrash.exe fails when compiled for x64/Release.  Making
"j" volatile avoids the problem.  Trying to catch the exception causes
an ICE ("LINK : fatal error C1001: An internal error has occurred in
the compiler.").

The following seems to get really confused when dealing with the"j".
The code generated near the problem looks like this:
```assembly
if (j < 0 || j > size- 2)
00007FF6748C1767  js          test_holder::test::crash+3B2h (07FF6748C1872h)  
00007FF6748C176D  cmp         edx,r15d  
00007FF6748C1770  jg          test_holder::test::crash+3B2h (07FF6748C1872h)  

        if (size - 2 >= permutation_.size())
00007FF6748C1776  mov         r8,qword ptr [r12]  
00007FF6748C177A  mov         rcx,qword ptr [r12+8]  
00007FF6748C177F  sub         rcx,r8  
00007FF6748C1782  sar         rcx,2  
00007FF6748C1786  movsxd      rax,r15d  
00007FF6748C1789  cmp         rax,rcx  
00007FF6748C178C  jae         test_holder::test::crash+38Fh (07FF6748C184Fh)  

        if (j < 0 || j >= permutation_.size())
00007FF6748C1792  movsxd      rax,edx  

        if (j < 0 || j >= permutation_.size())
00007FF6748C1795  mov         r9,0FFFFFFFF00000000h  
00007FF6748C179F  add         r9,rax  
```
What is adding 0xFFFFFFFF00000000 to r9 supposed to do?

```assembly
00007FF6748C17A2  cmp         r9,rcx  
00007FF6748C17A5  jae         test_holder::test::crash+354h (07FF6748C1814h)  

        std::swap(permutation_[i], permutation_[j]);
00007FF6748C17A7  mov         ecx,dword ptr [r14+r8]  
00007FF6748C17AB  mov         eax,dword ptr [r8+r9*4]  
00007FF6748C17AF  mov         dword ptr [r14+r8],eax  
00007FF6748C17B3  mov         dword ptr [r8+r9*4],ecx
```
