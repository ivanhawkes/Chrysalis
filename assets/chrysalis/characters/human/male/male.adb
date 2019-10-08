<AnimDB FragDef="chrysalis/characters/human/male/male_fragment_ids.xml" TagDef="chrysalis/characters/human/male/male_tags.xml">
  <FragmentList>
    <Idle>
      <Fragment BlendOutDuration="0.2" Tags="Alerted+ScopeLookPose">
        <ProcLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.30000001"/>
          <Procedural type="Looking">
            <ProceduralParams CryXmlVersion="2"/>
          </Procedural>
        </ProcLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="Unaware">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="stand_relaxed_idle" flags="Loop"/>
        </AnimLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="Crouching+ScopeLookPose">
        <ProcLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.30000001"/>
          <Procedural type="Looking">
            <ProceduralParams CryXmlVersion="2"/>
          </Procedural>
        </ProcLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="Crouching">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="crouch_idle"/>
        </AnimLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="Crouching">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="crouch_idle_v2"/>
        </AnimLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="ScopeLookPose">
        <ProcLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.30000001"/>
          <Procedural type="Looking">
            <ProceduralParams CryXmlVersion="2"/>
          </Procedural>
        </ProcLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="stand_relaxed_idle" flags="Loop"/>
        </AnimLayer>
      </Fragment>
    </Idle>
    <Move>
      <Fragment BlendOutDuration="0.2" Tags="Crouching">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="bspace_2d_move_strafe_crouch" flags="Loop"/>
        </AnimLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="ScopeLookPose">
        <ProcLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.30000001"/>
          <Procedural type="Looking">
            <ProceduralParams CryXmlVersion="2"/>
          </Procedural>
        </ProcLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="bspace_2d_move_strafe" flags="Loop"/>
        </AnimLayer>
      </Fragment>
    </Move>
    <Interaction>
      <Fragment BlendOutDuration="0.2" Tags="ScopeSlave">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="crouchwalk_l"/>
        </AnimLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="ScopeLookPose">
        <ProcLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.30000001"/>
          <Procedural type="Looking">
            <ProceduralParams CryXmlVersion="2"/>
          </Procedural>
        </ProcLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="picking_up_object"/>
        </AnimLayer>
      </Fragment>
    </Interaction>
    <Emote>
      <Fragment BlendOutDuration="0.2" Tags="Awe">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="crouch_r_90"/>
        </AnimLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="Apathy">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="crouch_l_135"/>
        </AnimLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="ScopeLookPose">
        <ProcLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.30000001"/>
          <Procedural type="Looking">
            <ProceduralParams CryXmlVersion="2"/>
          </Procedural>
        </ProcLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags="">
        <AnimLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
          <Animation name="crouch_to_crouchwalk_b"/>
        </AnimLayer>
      </Fragment>
    </Emote>
    <Looking>
      <Fragment BlendOutDuration="0.2" Tags="ScopeLooking">
        <ProcLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.30000001"/>
          <Procedural type="Looking">
            <ProceduralParams CryXmlVersion="2"/>
          </Procedural>
        </ProcLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags=""/>
    </Looking>
    <LookPose>
      <Fragment BlendOutDuration="0.2" Tags="ScopeLookPose">
        <ProcLayer>
          <Blend ExitTime="0" StartTime="0" Duration="0.30000001"/>
          <Procedural type="Looking">
            <ProceduralParams CryXmlVersion="2"/>
          </Procedural>
        </ProcLayer>
      </Fragment>
      <Fragment BlendOutDuration="0.2" Tags=""/>
    </LookPose>
  </FragmentList>
</AnimDB>
