// Shuffle.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <memory>
#include <random>
#include <vector>
#include <string>
#include <iostream>

class test_holder
{
public:
    explicit test_holder(int count)
    {
        tests_.reserve(count);

        for (auto i = 0; i < count; ++i)
            tests_.push_back(std::make_unique<test>(*this, i));
    }

    bool crash(int id) { return tests_[id]->crash(); }
private:
    class test
    {
        test_holder& holder_;
        std::vector<int> permutation_;
        int id_;
        std::mt19937 rng_;
    public:
        test(test_holder& holder, int id) : holder_{ holder }, id_{ id }
        {
        }

        bool crash();
    };

    std::vector<std::unique_ptr<test>> tests_;
};


bool test_holder::test::crash()
{
    const auto size = static_cast<int>(holder_.tests_.size());

    if (size < 2)
        return false;

    if (permutation_.size() != size - 1)
    {
        permutation_.clear();
        permutation_.reserve(size - 1);

        for (auto id = 0; id < size; ++id)
        {
            if (id == id_)
                continue;

            permutation_.push_back(id);
        }

        std::shuffle(permutation_.begin(), permutation_.end(), rng_);
    }

    // Fisher-Yates, but stop as soon as we find a valid peer.
    for (auto i = 0; i < size - 1; ++i)
    {
        // Uncomment the "volatile" to get things to work.
        //volatile
        const auto j = std::uniform_int_distribution<int>{ i, size - 2 }(rng_);

        if (j < 0 || j > size- 2)
            throw std::range_error("j out of range");

        if (size - 2 >= permutation_.size())
            throw std::range_error("size mismatch");

        if (j < 0 || j >= permutation_.size())
            throw std::range_error("Huh..? j = " + std::to_string(j));

        std::swap(permutation_[i], permutation_[j]);

        const auto id = permutation_[i];

        if (holder_.tests_[id])
            return true;
    }

    return false;
}


int main()
{
    //try
    //{
        test_holder t{ 150 };

        for (auto i = 0; i < 150; ++i)
            t.crash(i);
    //}
    //catch (const std::exception& ex)
    //{
        //std::cout << "Exception: " << ex.what() << '\n';
    //}

/* Uncommment the above try/catch to get:
1>------ Build started: Project: Shuffle, Configuration: Release x64 ------
1>Shuffle.cpp
1>Generating code
1>LINK : fatal error C1001: An internal error has occurred in the compiler.
1>(compiler file 'f:\dd\vctools\compiler\utc\src\p2\main.c', line 258)
1> To work around this problem, try simplifying or changing the program near the locations listed above.
1>Please choose the Technical Support command on the Visual C++
1> Help menu, or open the Technical Support help file for more information
1>  link!InvokeCompilerPass()+0x1f1b9
1>  link!InvokeCompilerPass()+0x1f1b9
1>  link!InvokeCompilerPass()+0x1ebd3
1>  link!DllGetC2Telemetry()+0x10769b
1>
1>LINK : fatal error C1001: An internal error has occurred in the compiler.
1>(compiler file 'f:\dd\vctools\compiler\utc\src\p2\main.c', line 258)
1> To work around this problem, try simplifying or changing the program near the locations listed above.
1>Please choose the Technical Support command on the Visual C++
1> Help menu, or open the Technical Support help file for more information
1>  link!InvokeCompilerPass()+0x1f1b9
1>  link!InvokeCompilerPass()+0x1ebd3
1>  link!DllGetC2Telemetry()+0x10769b
1>
1>
1>LINK : fatal error LNK1000: Internal error during IMAGE::BuildImage
1>
1>  Version 14.12.25831.0
1>
1>  ExceptionCode            = C0000005
1>  ExceptionFlags           = 00000000
1>  ExceptionAddress         = 066536E6 (06220000) "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.12.25827\bin\HostX86\x64\c2.dll"
1>  NumberParameters         = 00000002
1>  ExceptionInformation[ 0] = 00000000
1>  ExceptionInformation[ 1] = 0D649468
1>
1>CONTEXT:
1>  Eax    = 0D649468  Esp    = 00D3EA38
1>  Ebx    = 036300EC  Ebp    = 00D3EA68
1>  Ecx    = 00000005  Esi    = 0D649468
1>  Edx    = 090B402C  Edi    = 00D3EA48
1>  Eip    = 066536E6  EFlags = 00010202
1>  SegCs  = 00000023  SegDs  = 0000002B
1>  SegSs  = 0000002B  SegEs  = 0000002B
1>  SegFs  = 00000053  SegGs  = 0000002B
1>  Dr0    = 00000000  Dr3    = 00000000
1>  Dr1    = 00000000  Dr6    = 00000000
1>  Dr2    = 00000000  Dr7    = 00000000
1>Done building project "Shuffle.vcxproj" -- FAILED.
========== Build: 0 succeeded, 1 failed, 0 up-to-date, 0 skipped ==========
*/

    return 0;
}
