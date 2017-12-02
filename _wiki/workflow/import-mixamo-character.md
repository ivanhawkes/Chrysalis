# Design

# Process the character using the FBXTool

*   Open a Windows Power Shell and navigate to the folder where you checked out fbxtool.
*   If you haven't built the code, do this now.
*   Run the fbxtool to remove the Mixamo namespace, and add the Root and RootProxy nodes.

# From FBXTool into Maya

*   Set the system to Z-up and 30 FPS.
*   Group all the blendshapes into a single group with CTRL-G; name it 'BlendShapes'.
*   Click on 'Hips' and press SHIFT-P to detach them from the 'Root'.
*   Zero the rotation on 'RootProxy', it doesn't belong there.
*   Create a scene root, facing the model's front.
*   Apply the same rotation onto 'Root' as the 'SceneRoot' has when facing in the same direction as the model. If you followed these instructions, that should be 180 degrees around the Z axis.
*   Middle mouse drag the 'Hips' back onto the 'Root'.
*   Create an export node on the 'Root', of type CHR and call it 'human_female'.
*   Drag the 'Meshes' group and drop it onto 'human_female_group'.
*   Select all the meshes and use the 'MAT.ED' to 'Create Group From Selection' and call it 'human_female'.
*   Using the 'Rigging' workspace, "Skin...Prune Small Weights" if you have too many skinning influences.
*   Validate and export it.

Save this as a Maya (*.mb) file.

*   Re-parent 'Root' onto 'RootProxy'.
*   Make sure you zero out the rotation on the 'Root' node joint and transfer it to the 'RootProxy' instead. Keep the model facing the same direction as it was when you exported the CHR file.
*   'Export Selection' to an FBX file. I typically call it 'human_male_base.fbx' or something similar to indicate it's purpose. Use that as input into Motionbuilder.

# Open up MotionBuilder

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

# Creating Animsettings Files

CRYENGINE has been know to fail to create the animsettings files needed for the animations to be used within the engine.

If you find this happens, you can use a Powershell script to create a new animsettings file for each i_caf in a given folder.

ls -Recurse -Filter "*.i_caf" | foreach { cp ~/Documents/animsettings.json "$($_.DirectoryName)/$($_.BaseName).animsettings"}

In order for this to work, you need to have created a file called 'animsettings.json' and placed it in the 'Documents' folder for the current Windows user e.g.

notepad ~/Documents/animsettings.json

Once that's done, open a PowerShell and cd to the folder with all the animations. Run that script.

BE WARNED: It is a destructive process and will overwrite any animsettings files that exist in that folder and any sub-folder.