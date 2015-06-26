--
-- Spencer Van Leeuwen
-- 20412199
-- puppet.lua
--

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)

function makeUpperBody(puppet)
    upperBody = gr.joint('upper body', {-30, 0, 45}, {0, 0, 0})
    puppet:add_child(upperBody)
    upperBody:translate(0.0, 6.0, 0.0)

    shoulders = gr.sphere('shoulders')
    upperBody:add_child(shoulders)
    shoulders:scale(6.0, 1.25, 1.25)
    shoulders:set_material(blue)

    makeLeftArm(upperBody)
    makeRightArm(upperBody)
    makeHead(upperBody)
end

function makeLeftArm(upperBody)
    -- Joint Nodes
    leftShldr = gr.joint('left shoulder', {-90, 0, 60}, {0, 0, 0})
    upperBody:add_child(leftShldr)
    leftShldr:translate(-6.0, 0.0, 0.0)

    leftElbow = gr.joint('left elbow', {-90, 0, 5}, {0, 0, 0})
    leftShldr:add_child(leftElbow)
    leftElbow:translate(0.0, -6.0, 0.0)

    leftWrist = gr.joint('left wrist', {-60, 0, 30}, {0, 0, 0})
    leftElbow:add_child(leftWrist)
    leftWrist:translate(0.0, -6.0, 0.0)

    -- Geometry Nodes
    leftUArm = gr.sphere('left upper arm')
    leftShldr:add_child(leftUArm)
    leftUArm:translate(0.0, -3.0, 0.0)
    leftUArm:scale(1.0, 3.25, 1.0)
    leftUArm:set_material(blue)

    leftLArm = gr.sphere('left lower arm')
    leftElbow:add_child(leftLArm)
    leftLArm:translate(0.0, -3.0, 0.0)
    leftLArm:scale(1.0, 3.5, 1.0)
    leftLArm:set_material(blue)

    leftHand = gr.sphere('left hand')
    leftWrist:add_child(leftHand)
    leftHand:translate(0.0, -1.0, 0.0)
    leftHand:scale(0.5, 2.0, 0.5)
    leftHand:set_material(blue)
end

function makeRightArm(upperBody)
   -- Joint Nodes
    rightShldr = gr.joint('right shoulder', {-90, 0, 60}, {0, 0, 0})
    upperBody:add_child(rightShldr)
    rightShldr:translate(6.0, 0.0, 0.0)

    rightElbow = gr.joint('right elbow', {-90, 0, 5}, {0, 0, 0})
    rightShldr:add_child(rightElbow)
    rightElbow:translate(0.0, -6.0, 0.0)

    rightWrist = gr.joint('right wrist', {-60, 0, 30}, {0, 0, 0})
    rightElbow:add_child(rightWrist)
    rightWrist:translate(0.0, -6.0, 0.0)

    -- Geometry Nodes
    rightUArm = gr.sphere('right upper arm')
    rightShldr:add_child(rightUArm)
    rightUArm:translate(0.0, -3.0, 0.0)
    rightUArm:scale(1.0, 3.5, 1.0)
    rightUArm:set_material(blue)

    rightLArm = gr.sphere('right lower arm')
    rightElbow:add_child(rightLArm)
    rightLArm:translate(0.0, -3.0, 0.0)
    rightLArm:scale(1.0, 3.5, 1.0)
    rightLArm:set_material(blue)

    rightHand = gr.sphere('right hand')
    rightWrist:add_child(rightHand)
    rightHand:translate(0.0, -1.0, 0.0)
    rightHand:scale(0.5, 2.0, 0.5)
    rightHand:set_material(blue)
end

function makeHead(upperBody)
    -- Joint Nodes
    neckJoint = gr.joint('neck joint', {-20, 0, 45}, {0, 0, 0})
    upperBody:add_child(neckJoint)

    headJoint = gr.joint('head joint', {-45, 0, 45}, {-75, 0, 75})
    neckJoint:add_child(headJoint)
    headJoint:translate(0.0, 5.0, 0.0)

    -- Geometry Nodes
    neck = gr.sphere('neck')
    neckJoint:add_child(neck)
    neck:translate(0.0, 2.0, 0.0)
    neck:scale(0.75, 2.0, 0.75)
    neck:set_material(blue)

    head = gr.sphere('head')
    headJoint:add_child(head)
    head:scale(2.5, 2.5, 2.5)
    head:set_material(blue)

    nose = gr.sphere('nose')
    head:add_child(nose)
    nose:translate(0.0, 0.0, 1.0)
    nose:scale(0.5/2.5, 0.5/2.5, 0.5/2.5)
    nose:set_material(red)
end

function makeLowerBody(puppet)
    lowerBody = gr.joint('lower body', {-60, 0, 10}, {0, 0, 0})
    puppet:add_child(lowerBody)
    lowerBody:translate(0.0, -6.0, 0.0)

    hips = gr.sphere('hips')
    lowerBody:add_child(hips)
    hips:scale(3.0, 1.5, 1.5)
    hips:set_material(blue)

    makeLeftLeg(lowerBody)
    makeRightLeg(lowerBody)
end

function makeLeftLeg(lowerBody)
    -- Joint Nodes
    leftHip = gr.joint('left hip', {-90, 0, 30}, {0, 0, 0})
    lowerBody:add_child(leftHip)
    leftHip:translate(-3.0, 0.0, 0.0)

    leftKnee = gr.joint('left knee', {-5, 0, 90}, {0, 0, 0})
    leftHip:add_child(leftKnee)
    leftKnee:translate(0.0, -6.0, 0.0)

    leftAnkle = gr.joint('left ankle', {-30, 0, 60}, {0, 0, 0})
    leftKnee:add_child(leftAnkle)
    leftAnkle:translate(0.0, -6.0, 0.0)

    -- Geometry Nodes
    leftThigh = gr.sphere('left thigh')
    leftHip:add_child(leftThigh)
    leftThigh:translate(0.0, -3.0, 0.0)
    leftThigh:scale(1.0, 3.5, 1.0)
    leftThigh:set_material(blue)

    leftCalf = gr.sphere('left calf')
    leftKnee:add_child(leftCalf)
    leftCalf:translate(0.0, -3.0, 0.0)
    leftCalf:scale(1.0, 3.5, 1.0)
    leftCalf:set_material(blue)

    leftFoot = gr.sphere('left foot')
    leftAnkle:add_child(leftFoot)
    leftFoot:translate(0.0, 0.0, 2.0)
    leftFoot:scale(1.0, 1.0, 2.0)
    leftFoot:set_material(blue)
end

function makeRightLeg(lowerBody)
     -- Joint Nodes
    rightHip = gr.joint('right hip', {-90, 0, 30}, {0, 0, 0})
    lowerBody:add_child(rightHip)
    rightHip:translate(3.0, 0.0, 0.0)

    rightKnee = gr.joint('right knee', {-5, 0, 90}, {0, 0, 0})
    rightHip:add_child(rightKnee)
    rightKnee:translate(0.0, -6.0, 0.0)

    rightAnkle = gr.joint('right ankle', {-30, 0, 60}, {0, 0, 0})
    rightKnee:add_child(rightAnkle)
    rightAnkle:translate(0.0, -6.0, 0.0)

    -- Geometry Nodes
    rightThigh = gr.sphere('right thigh')
    rightHip:add_child(rightThigh)
    rightThigh:translate(0.0, -3.0, 0.0)
    rightThigh:scale(1.0, 3.5, 1.0)
    rightThigh:set_material(blue)

    rightCalf = gr.sphere('right calf')
    rightKnee:add_child(rightCalf)
    rightCalf:translate(0.0, -3.0, 0.0)
    rightCalf:scale(1.0, 3.5, 1.0)
    rightCalf:set_material(blue)

    rightFoot = gr.sphere('right foot')
    rightAnkle:add_child(rightFoot)
    rightFoot:translate(0.0, 0.0, 2.0)
    rightFoot:scale(1.0, 1.0, 2.0)
    rightFoot:set_material(blue)
end

puppet = gr.node('root')

torso = gr.sphere('torso')
puppet:add_child(torso)
torso:scale(3.0, 6.0, 3.0)
torso:set_material(blue)

makeUpperBody(puppet)
makeLowerBody(puppet)

return puppet
