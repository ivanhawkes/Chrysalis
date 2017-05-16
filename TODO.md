#Present Work

Think about CCharacter becoming CHuman[oid], inherited from CCharacter. Doing so will allow a CHumanoidRig file with lists of bones and things needed to drive the animation rig. Similar for other attributes - lists of consts and enums that are frequently used or best all gathered in one place e.g.

-CActor
--CCharacter
---CHumanoid
----CHuman
-----CHumanRig
-----CHumanConst (enums, whatever - better name than this)
----CAlien
-----CAlienRig
-----CAlienConst (enums, whatever - better name than this)

#HMD

Conceptually, a head mounted display should be considered an absolute controller. The camera position and rotation needs to respond to it's inputs without delay or quibble - or you will cause nausea.

Either the first person camera, or a specialised VR one will need to track head movements ruthlessly - and perhaps black / fade the screen if you start to wall peek or commit other silliness.

Given the tether, use of such HMD as delta movement and rotation should not be considered a cheat - and instead treated as sincere input.

While it's possible to supply specific VR cameras for FP and TP views, it might just be more sensible to apply a few offsets and extra quaternions when VR is in use.