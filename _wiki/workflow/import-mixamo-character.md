#Design

#Process the character using the FBXTool

*   Open a Windows Power Shell and navigate to the folder where you checked out fbxtool.
*   If you haven't built the code, do this now.
*   Run the fbxtool to remove the Mixamo namespace, and add the Root and RootProxy nodes e.g.

```
.\bin\x64\Debug\fbxtool.exe -i human_male.fbx -o D:\Chrysalis\assets_source\objects\characters\human\male\human_male.fbx -v -j .\mixamo-to-autodesk.json -f
```


#From FBXTool into Maya

*   Before you open the model, be sure to set Maya to use Z-up and 30 FPS for animation.
*   Group all the blendshapes into a single group with CTRL-G; name it 'BlendShapes'. This just gets them all out of the way.

#Fix the Rotations.

Our end goal is to have all rotations on 'Root' and 'RootProxy' set to 0, but also for their local rotation to align with the world orientation, which is a Z-up right handed system. We also want to spin the model 180 degrees around Z, without changing it's already set orientations. This will leave it facing forward with respect to the world orientation.

We can do this by parenting it to either the 'Root' or 'RootProxy' and applying the rotations on them, de-parenting, then removing them again. This seems to apply it to an internal rotation instead (perhaps the pre-rotation that the FBX files contain).

*   Click on 'Hips' and press SHIFT-P to detach them from the 'Root'. This is to prevent any rotations we are about to perform on the 'Root' from cascading down.
*   Click on 'Root' and press SHIFT-P to detach it from the 'RootProxy'.

There is a little bit of tricky stuff needed here.

*   Zero the rotation on 'RootProxy', it doesn't belong there.
*   Click on the 'Root' and using the Attribute Editor...Display tell it to 'Display Local Axis'. Collin Bishop made a video about this which you can find on YouTube / Vimeo if you need more detail.
*   Apply whatever rotation is needed on the 'RootProxy' to align the 'Root' (local axis) with the world orientation. 'Root' should have a zero rotation at this point. De-parent 'Root' once this is done. Zero out the rotation on 'RootProxy'.
*   Middle mouse drag the 'Hips' onto 'RootProxy'. Now apply a rotation to 'RootProxy' that aligns the character so they are facing forward in the Z-Up orientation. De-parent 'Hips' once this is done. Zero out the rotation on 'RootProxy'.
*   At this point 'Root' and 'Hips' should both be facing in the correct orientation. 'Root' should have zero rotations, and 'Hips' should be zero or close to zero - whatever the model had originally.
*   Middle mouse drag the 'Hips' back onto the 'Root'.

#Setting up for Export

*   Create a CHR export node on the 'Root', and call it 'human_female'.
*   Drag the 'Meshes' group and drop it onto 'human_female_group'.
*   Select all the meshes and use the 'MAT.ED' to 'Create Group From Selection' and call it 'human_female'.
*   Validate the model - it likely has skinning weight errors from having too many skinning weights. Use the 'Focus' button to focus each vertex that has an error. Using the 'Rigging' workspace, "Skin...Prune Small Weights". This should remove enough weights for you to validate correctly now.

Save this as a Maya (*.mb) file.

*   Validate and export it.
*   Re-parent 'Root' onto 'RootProxy'.
*   Make sure you zero out the rotation on the 'Root' node joint and transfer it to the 'RootProxy' instead. Keep the model facing the same direction as it was when you exported the CHR file.
*   'Export Selection' to an FBX file. I typically call it 'human_male_base.fbx' or something similar to indicate it's purpose. Use that as input into Motionbuilder.

#Open up MotionBuilder

NOTE: Before doing this section it's worth taking the time to watch the awesome video the Motus guys made to step you through re-targeting character animations.

https://www.youtube.com/watch?v=bS_ze_ASmak&feature=youtu.be

*   Drag a character onto the root of the target to characterise the skeleton.
*   Rename to 'Mixamo Character'.
*   Make sure Mixamo Character Root is characterised as the 'Reference' node. If not, do that now.
*   Create a new character extension for that Mixamo Character
*   Alt drag the Mixamo Character 'Root' onto the character extension. Add it.

SAVE A COPY OF THE FILE TO A NEW FBX AT THIS POINT

e.g. human_female_base.fbx

*   File...Merge the source animations + skeleton + mesh into the scene.
*   Set the Mixamo Character to use the Motus Character as the target source.
*   Set the Mixamo Character to use 'Retargeting...Match Source'.
*   Drag a 'Parent Child' constraint onto the Mixamo Character root. Set the Motus 'motus:Reference' as it's parent. Uncheck the 'Translation Y' option. Snap the constraint while in the T-Pose.
*   Select the 'RootProxy' null node and scale it so the Mixamo model is the same size (1.14 for human female), or as close as possible, to the Motus skeleton.

For safety, save out a copy of the work now, dropping any extra animations at this time if needed. Load the newly saved file if you dumped any animations.

*   Plot (All Takes) the animations onto the Mixamo skeleton.
*   Switch the parent child constraint to inactive.
*   Your animations should be plotted correctly at this point and the root should be under the Mixamo Character as expected. If not, work through the steps and make sure you have them all correct.
*   Delete all the Motus geometry, skeletons, and characters. You should now have only the Mixamo character and the animations plotted onto it.
*   Scale 'RootProxy' node back to 1.0, 1.0, 1.0. Everything should now be ready to export.
*   'Open Reality...Tools...CryEngine 3 Exporter...Export Animations'.
*   Copy the exported animations into your CRYENGINE project.
*   Generate new *.animsettings files if you must by hand (if the engine fails to do so).

#Creating Animsettings Files

CRYENGINE has been know to fail to create the animsettings files needed for the animations to be used within the engine.

If you find this happens, you can use a Powershell script to create a new animsettings file for each i_caf in a given folder.

```
ls -Recurse -Filter "*.i_caf" | foreach { cp ~/Documents/animsettings.json "$($_.DirectoryName)/$($_.BaseName).animsettings"}
```

In order for this to work, you need to have created a file called 'animsettings.json' and placed it in the 'Documents' folder for the current Windows user e.g.

```
notepad ~/Documents/animsettings.json
```

Once that's done, open a PowerShell and cd to the folder with all the animations. Run that script.

BE WARNED: It is a destructive process and will overwrite any animsettings files that exist in that folder and any sub-folder.