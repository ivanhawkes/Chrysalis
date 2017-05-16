# Chrysalis C++ Coding Standard # 

The purpose of in-house coding standards is to set a clear style of coding for the whole team, making it easier for team members to co-operate and understand each other's code.

Where possible, code should be self documenting. This is achieved through a combination of the use of industry standard practice, long descriptive names, and well written text comments. By opting to use common design patterns or well understood methods of solving standard problems we increase the readability of the code.

In part, we take our coding cues from the existing CRYENGINE source code, not because they have implemented the best standards, but because we want our code to harmonise with CryTek code as much as is reasonable.

## Constant Refactoring ##

We will be utilising large blocks of code from the existing sample SDK game. At times it will be tempting to rename variables and structs, simply to bring them into line with our arbitrary standard. Before you do, exmine the impact the renaming will have on both our codebase (which should be nil or minimal) and the sample SDK codebase; in particular if this makes a lot of work for someone else needing other code snippets that make use of the older names.

It can be better to simply use the code you are copying "as-is" until you have completed moving all affected code into our codebase. We can always refactor names quite easily at a later point.

If you see any "quick wins", such as parameter names, variables in block scope, or names which don't impact other systems, then by all means - rename them as you move the code into our project.

The CRYENGINE code suffers from "too many hands", so as you move in blocks of code you are expected to refactor that code before merging it in with the rest of our code. At a minimum you will need to apply our document formatting standard (set within MSVC and applied using CTRL-k-d). Clean up extraneous spacing, and apply block spacing according to our specifications.

Write comments for any code whose function you have determined, but is currently without a block comment.

Remove any useless banner headers from the code, as this is useless cruft. If the banner had a file description comment, then transfer that to our standard file description header format.

## Comments ##

Comments are critical to the long term readability of code. There are some good online sources on how to effectively comment your code. A quick Google search is all you really need here. Consult the code to see how others have written their comments.

It is preferred for every block of code to have a comment on the line(s) preceeding the block. Note: this is prefererred even for single lines of code, although the comment itself is quite often redundant, as it breaks the code up. Programmers can skim the green comments much quicker than reading each line in many cases.

While short lines / blocks of code can get away with a very quick description of what the line will achieve longer blocks should focus on both what it will do, why, anything you need to look out for, any preconditions, anything that is different about this block compared to others, etc. We can find out how you did something by reading the code...everything else you want to convey goes into the comments.

e.g. a line of code calls for using a bubble sort. Many programmers will think that's not efficient and switch it to a quicksort without considering the specifics of this code. A single line of comment might note that bubble sort is being used because there are only a few items or some other reason where that sort is faster than the standard quicksort.

Comments need to be human readable. We also need to make them machine readable for documentation purposes, but machines are much better at reading code than we are - so we focus on making comment blocks easy for a programmer to read. We are favouring an open style, with minimal amount of tags / blocks, and auto-formatted (using an AddOn) to provide nice features such as lining up parameter descriptions.

Favour using comments that can be automatically parsed by the documentation system we are using, Doxygen.

Prefer using one line commnents where possible.

I highly recommend all programmers purchase a copy of Atomineer Pro, set it with our settings, and use that to auto-comment their functions, members, etc. It's money well spent on a tedious and time consuming task that will make them far more realible, far faster to create and far easier to confirm / maintain.

## Initializer Function Format ##


   TRMIInventoryAmmo( const char* pAmmoClass, int iAmount )
     : m_AmmoClass ( pAmmoClass )
     , m_iAmount ( iAmount )
   {}

## Naming Conventions ##

* Variable, function, state, and class names should be clear, unambiguous, and descriptive. The greater the scope of the name, the greater the importance of a good, descriptive name. Avoid over-abbreviation.

* All variables should be declared one at time so that a comment on the meaning of the variable can be provided. This helps out auto documentation tools also.

* All names begin with a letter, and may contain any ASCII alpha-numerals. Names should not include underscores, with the exception of #defines and constants that are defined using the old C style of all uppercase letters.

* Classes. Pascal case, with a capital 'C' placed at the beginning of the name e.g. CMyClass

* Interface classes. Pascal case, with a capital 'I' placed at the beginning of the name e.g. IMyInterface
 
* Structs. Pascal case, with a capital 'S' placed at the beginning of the name e.g. SMyStruct

* Methods. Pascal case. e.g. MyMethod

* Variables. Camel case. Begin with a lowercase letter but capitalize each word there after e.g. localVariable

* Variables begin with a scope prefix and an underscore. The following part should follow camel case:
** g_globalVariable
** m_classMemberVariable
** s_staticClassMemberVariable
** s_moduleVariable (a file static variable. s for “static”/anonymous namespace declared at file scope, not globally accessible yet not a member). 
* Local variables do not get a scope prefix: localVariable

* Parameter names do not get a scope prefix and follow camel case e.g. (int weaponCount)

* Macros are capitalized and use underscores: MY_MACRO

* Enums are currently a nightmare mix of all styles forced on us by CRYENGINE. preferrence:
** Declaration name begin with a capital 'E' followed by the actual name in Pascal case
** Member names are prefixed with a unique name part which is shared, followed by an underscore, and then an individual name part
** NOTE: it would be great to use the new enum features in c++11 instead where possible with scope resolution on the names.

* All booleans should ask a true/false question. All functions that return a bool should do the same. 

* Class names are nouns, Method names are verbs

* Use of plural or singular is at programmer discretion, but it should convey of sense of whether something is plural or singular and sound right when spoke outload.

## Header Files ##

* use #pragma once instead of the old #define guard. Though not officially standard, works on just about everything now.

* Don't use an <code>#include</code> when a forward declaration would suffice.

* Put a module's own .h before any other includes to enforce correct forward declaration and prevent hidden dependancy chains

* Use #include < > to include files unless the use of quotes (relative path lookup) is required.

## Const ##

* Whenever you can use const in C++, use const. Particularly on function parameters and class methods. const is documentation as much as it is a compiler directive.

* Anything passed by reference as an input to a function should be declared const.

## Types ##

* Prefer native "bool" in logical code.

* Prefer native "int" in logic code where size really doesn't matter

* When specifying a data structure that will be serialized (such as a network message or any data that is going to disk) you must use the size precise types in Type.h such as Int32, Byte, Bool etc.  An easy rule to remember is that when serializing, all your data members types must be our custom ones that start with a capital letter. (NOTE: probably not relevant - but applies in kind to serialised data in any case)

## What #ifdef to use and when ##

Also, as we move forward … if you are working on enabling/disabling a whole system of code which spans one or even multiple source files, please make a #define for that functionality prefixed with “WITH_” and add it to the appropriate section of Preprocessors.h.  For instance, Preprocessors has the following defined for DEBUG configurations:

#if defined(DEBUG)	
	#define WITH_SOME_DEFINE_HERE 1
	#define WITH_ANTOHER_DEFINE_HERE 1
#endif


## x64 ##

Code should be 64-bit and 32-bit friendly. Bear in mind problems of printing, comparisons, and structure alignment.

When printing 64-bit id’s in verify message strings or using any other printf-style format syntax:
* Prefer using “%llu” (unsigned long long). You probably don't want "%lu" for unsigned long which changes depending on the compiler.
* Do NOT use “%I64i” for signed, “%I64u” for unsigned, or “%I64x” for hex.

## 0 and NULL ##

Use 0 for integers, 0.0f for floats, nullptr for pointers, and '\0' for chars.

## Classes and Inheritance ##

* In general, constructors should merely set member variables to their initial values. Any complex initialization should go in an explicit Init() method.

* You must define a default constructor if your class defines member variables and has no other constructors. Otherwise the compiler will do it for you, badly.

* Use a struct only for passive objects that carry data; everything else is a class.

* Try to avoid deep C++ hierarchies.

* When declaring a virtual function in a derived class that overrides a virtual function in the parent class, you must use the virtual keyword. Although it is optional according to the C++ standard because the 'virtual' behaviour is inherited, code is much clearer if all virtual function declarations use the virtual keyword.

* Only very rarely is multiple implementation inheritance actually useful. We allow multiple inheritance only when at most one of the base classes has an implementation; all other base classes must be pure interface classes.

* Prefer to declare members of a class in this order: Public, Protected, Private. There are occasional valid reasons to deviate from this (e.g. private constructors).

* For very large classes it is permissable to break the header up using block comments into logical sections. Each logical section can declares the methods, members and enums it requires and make use of public, protected, private. In a perfect world, this code would be in another class or a component or merged in using composition, but our world isn't perfect.

## Memory Allocation ## 

* Think carefully about memory thrashing, heap exhaustion and other issues when allocating memory.

* Use fixed pools of memory where possible to make memory usage bounded.

* Use fixed width memory structures when needed for speed 

* Prefer the new c++11 features for automatic memory over auto_ptr.

## Exceptions ##

We do not use C++ exceptions in runtime code (client or server). It's OK to use these in tools code.

## Run-Time Type Information (RTTI) ##

We do not use Run Time Type Information (RTTI) in runtime code (client or server). It's OK to use it in tools code..

## STL ##

* Prefer the STL containers over custom-made containers. Exceptions to this should be very rare.

* STL containers must use our memory system allocators

* Use the simple containers by default. (Vector, List) Be sure that you need a map before you use it.

* All algorithms are available for use.

## Casting ##

Use C++ casts like static_cast<>(). Do not use other cast formats like int y = (int)x; or int y = int(x);.

## Execution Blocks ##

### braces { } ###
 
Braces go on a newline.

### if - else ###

* Each block of execution in an if-else statement should be in braces. This is to prevent editing mistakes - when braces aren't used, someone could unwittingly add another line to an if block. The line wouldn't be controlled by the if expression, which would be bad. Worse yet is when conditionally compiled items cause if/else statements to break. So always use braces.

* The else keyword belongs on a new line.

if (bHaveUnrealLicense)
{
   InsertYourGameHere ();
}
else
{
   CallMarkRein ();
}

A multi-way if statement should be indented with each else if indented the same amount as the first if; this makes the structure clear to a reader:


if (TannicAcid < 10)
{
   log ("Low Acid");
}
else if (TannicAcid < 100)
{
   log ("Medium Acid");
}
else
{
   log ("High Acid");
}

Note: simple if statements followed by a single line of simple code can omit the braces if desired e.g.

if (x < y)
    return;

## Tabs vs. Spaces ##

* Use tabs. A tab size of 4 is standard for us.

## Boolean expressions ##

Always use 'true' or 'false'.  Don't use 0 or 1 because they're not as clear to read.

## File I/O and Logging ##

Use the features provided by CRYENGINE.

## General style issues ##

* Avoid multiple returns from a function. Unless it will overly obfuscate your code or slow it down unreasonably, use a single return statement. This makes code much easier to maintain, as it's simpler to track the path of execution, make debugging changes, etc. Obvious exceptions are lines of code at the top of a function which "early out" e.g.

if (!pWeNeedThisOrWeDie)
    return;

* Minimize dependency distance. When code depends on a variable having a certain value, try to set that variable's value right before using it. Initializing a variable at the top of an execution block, and not using it for a hundred lines of code, gives lots of space for someone to accidentally change the value without realizing the dependency. Having it on the next line makes it clear why the variable is initialized the way it is and where it is used. A Berkeley study showed that a variable declared more than 7 lines from its usage had an extremely high chance (> 80%) of being used incorrectly.

Even better, initialise it and declare it using the same line, unless this undly impacts readability.

* Function length. Functions should be as long as they need to be to complete their task. Functions should be focused on a single well defined task, and it is that which will determine their length.

If you find your functions are growing unreasonably long, think about how the task can be refactored. Use smaller functions and inline functions to avoid call overhead.

In most cases, call overhead is not going to cause major performance issues, but it can inside critical sections that are called frequently. In this case, suck it up and refactor for speed, but only once you know where is a speed issue with the function that can be improved by doing this.

* Leave a blank line at the end of the file. All .cpp and .h files should include a blank line to play nice with gcc.

* Use whitespace wisely.

* Place two empty lines after each method block in the implementation files as this makes it easier to scan through the functions.

## Compiler Warnings ##

* Address compiler warnings. Compiler warning messages mean something is not as it should be. Fix what the compiler is complaining about. We will have our builds treat warnings as errors. 

* If you absolutely cannot address a warning, use #pragma to suppress the warning; this is a remedy of last resort and be put around the narrowest possible scope of code. Use the "push" and "pop" options on the #pragma disable to do this.

* Never use #pragma disable() in headers (as that can turn off warnings in other code), only in cpp files.

* Do not define _CRT_NONSTDC_NO_DEPRECATE.  When a warning pops regarding deprecated POSIX functions change the function name to the correct POSIX function (usually by prefixing with an underscore).

* Do not define _CRT_SECURE_NO_WARNINGS.  When a warning pops up regarding insecure CRT function calls change the function to the appropriate secure function call.  If that function is compiler specific (as some Microsoft provided ones are) then encapsulate the call with a #if to check for the proper compiler and offer a #elif for each other platform and a #else for all future platforms.  The default #else section should generate a compiler error to enforce that it will be fixed on that platform with appropriate secure code when the code is ported.

## Debug Code ##

Debug code should be wrapped #ifdef blocks to prevent it from making it's way into release code.

## Modifying Third Party Code ##

Whenever you modify the code to a library that we use in the engine, be sure to tag your changes with a // @Chrysalis comment, as well as an explanation of why you made the change. This make merging the changes into new version of that library easier, and lets licensees easily find any modifications we have made.