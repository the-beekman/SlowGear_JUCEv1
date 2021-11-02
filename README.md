# SlowGear_JUCEv1
An automatic volume swell effect controlled by an envelope follower. Written in C++(17) using the JUCE Framework to create AU and VST plug-ins.

The envelope follower is a low-pass filter that creates a "side-chain" signal. When the output of the envelope follower rises above the user-defined threshold, the effect starts multiplying the user input signal by a gain ramp. There are separate envelope follower filter coefficients for when the envelope is "rising" or "falling." The default parameters have a fast response to the initial attack of the signal and a smooth response when the signal stabilizes and decays. To prevent unwanted repeated swells if the envelope settles around the threshold, the envelope signal must fall below 85% of the threshold to put the plug-in in a "ready" state.



https://user-images.githubusercontent.com/88636127/139404153-57c54209-c8e6-437d-8fa1-dee0c1a5e6f3.mov


