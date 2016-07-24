# Spp2Pgs
Converts general subtitles to HDMV PG streams.  
In other words, converts .ass files to .sup files.

The project generates a command line application and a CLR dll file.  
Core of them is a static library, libspp2pgs.

An external dll, xy-VSSppf, is introduced to deal with subtitle files.  
It's based on a forked version of xy-VSFilter, rendering and providing subtitles in a more easy way.  

Here is a breif description for all the four parts.

## Spp2Pgs*.exe
An executable converts subtitles to .sup files.  
Usage:  
`Spp2Pgs -i "X:\Saya1011con.ass" -s 1080 -r 23 "X:\Saya1011con.sup"`  
Type Spp2Pgs -h for more help.

## libspp2pgs & Spp2PgsNet*.dll
The core and its CLR library wrapper for .NET framework applications.  
Three classes play the main role:  
* PgsEncoder, encoding and writing a pgs file.
* FrameStream, reading subtitles and rendering subtitles into image.
* Spp2Pgs, an entrance for all other things. Also reading images from FrameStream and sending them to PgsEncoder.

And this is a SAMPLE for C#:  
```csharp
//class MySettings : IS2PSettings { ... }
//class MyLogger : IS2PLogger { ... }

using (Spp2Pgs tS2P = new Spp2Pgs(new MySettings(), new MyLogger()))	//Create Spp2Pgs obj
using (var tCtx = tS2P.CreateSubPicProviderContext())	//Create SppContext Obj, necessary for creating Spp
using (var tOutputStream = new FileStream(@"X:\Sayas1011con.sup", FileMode.Create))	//Create output file
using (var tOutput = tS2P.CreatePgsEncoder(tOutputStream, BdViFormat.Vi1080i, BdViFrameRate.Vi23)) //Create PgsEncoder
{
    tOutput.RegistAnchor(0);    //Ensure an epoch at pts==0

	//Create Spp, necessary for processing subtitle files
	using (var tSpp = tS2P.CreateSubPicProvider(tCtx, new FileInfo(@"X:\Illya.ass")))
	{
		//Create Advisor, providing additional info for FrameStream.
		var tAdv =
			tS2P.CreateSppFrameStreamAdvisor(tSpp, BdViFormat.Vi1080p, BdViFrameRate.Vi23,	//Rendering format and rate
			-1, -1, 0);	//Rendering subtitle from begin to end, without an offset.
		//Create FrameStream
		using (var tInput = tS2P.CreateFrameStream(tSpp, tAdv))
		{
			//Encode from a FrameStream to a PgsEncoder
			tS2P.Encode(tInput, tOutput, null);
			//Done.
		}
	}
}
```

## xy-VSSppf*.dll
xy-VobSub SubPicProvider Factory. This is a COM dll, exposing an ISubPicProviderAlfa interface to Spp2Pgs.  
It's a necessary part for Spp2Pgs.


## Special Thanks
* This projects starts on basis of [avs2bdnxml](http://www.ps-auxw.de/avs2bdnxml/)
* xy-VSSppf is a forked version of [xy-VSFilter](https://github.com/Cyberbeing/xy-VSFilter)
