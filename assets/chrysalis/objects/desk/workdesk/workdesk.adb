<AnimDB FragDef="chrysalis/objects/desk/workdesk/fragment_ids.xml" TagDef="chrysalis/objects/desk/workdesk/tags.xml">
 <FragmentList>
  <Interact>
   <Fragment BlendOutDuration="0.2" Tags="ScopeSlave">
    <AnimLayer>
     <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
     <Animation name="drawer_top_open"/>
    </AnimLayer>
   </Fragment>
  </Interact>
  <Closed>
   <Fragment BlendOutDuration="0.2" Tags="ScopeSlave">
    <AnimLayer>
     <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
     <Animation name="door_close"/>
    </AnimLayer>
   </Fragment>
   <Fragment BlendOutDuration="0.2" Tags=""/>
  </Closed>
  <Open>
   <Fragment BlendOutDuration="0.2" Tags="ScopeSlave">
    <AnimLayer>
     <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
     <Animation name="drawer_bottom_open"/>
    </AnimLayer>
   </Fragment>
   <Fragment BlendOutDuration="0.2" Tags=""/>
  </Open>
  <Opening>
   <Fragment BlendOutDuration="0.2" Tags="ScopeSlave">
    <AnimLayer>
     <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
     <Animation name="drawer_top_open"/>
    </AnimLayer>
   </Fragment>
  </Opening>
  <Closing>
   <Fragment BlendOutDuration="0.2" Tags="ScopeSlave">
    <AnimLayer>
     <Blend ExitTime="0" StartTime="0" Duration="0.2"/>
     <Animation name="drawer_top_close"/>
    </AnimLayer>
   </Fragment>
   <Fragment BlendOutDuration="0.2" Tags=""/>
  </Closing>
 </FragmentList>
</AnimDB>
