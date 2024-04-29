//----------------------------------------------------------------------------//
//                                                                            //
// ozz-animation is hosted at http://github.com/guillaumeblanc/ozz-animation  //
// and distributed under the MIT License (MIT).                               //
//                                                                            //
// Copyright (c) Guillaume Blanc                                              //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// all copies or substantial portions of the Software.                        //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
//                                                                            //
//----------------------------------------------------------------------------//

#include "gtest/gtest.h"
#include "ozz/animation/runtime/motion_blending_job.h"
#include "ozz/base/maths/gtest_math_helper.h"
#include "ozz/base/maths/transform.h"

using ozz::animation::MotionBlendingJob;

TEST(Validate, MotionBlendingJob) {
  MotionBlendingJob job;
  EXPECT_FALSE(job.Validate());

  ozz::math::Transform output;
  job.output = &output;
  EXPECT_TRUE(job.Validate());

  MotionBlendingJob::Layer layers[2];
  job.layers = layers;
  EXPECT_FALSE(job.Validate());

  ozz::math::Transform transforms[2];
  layers[0].transform = &transforms[0];
  EXPECT_FALSE(job.Validate());

  layers[1].transform = &transforms[1];
  EXPECT_TRUE(job.Validate());
}

TEST(Empty, MotionBlendingJob) {
  MotionBlendingJob job;
  ozz::math::Transform output;
  job.output = &output;

  EXPECT_TRUE(job.Run());

  EXPECT_FLOAT3_EQ(output.translation, 0.f, 0.f, 0.f);
  EXPECT_QUATERNION_EQ(output.rotation, 0.f, 0.f, 0.f, 1.f);
  EXPECT_FLOAT3_EQ(output.scale, 1.f, 1.f, 1.f);
}

TEST(Run, MotionBlendingJob) {
  MotionBlendingJob job;
  ozz::math::Transform output;
  job.output = &output;

  ozz::math::Transform transforms[2];

  transforms[0].translation = {2.f, 0.f, 0.f};
  transforms[0].rotation = {.70710677f, 0.f, 0.f, .70710677f};

  transforms[1].translation = {0.f, 0.f, 3.f};
  transforms[1].rotation = {-0.f, -.70710677f, -0.f, -.70710677f};

  MotionBlendingJob::Layer layers[2];
  layers[0].transform = &transforms[0];
  layers[1].transform = &transforms[1];
  job.layers = layers;

  // 0 weights
  EXPECT_TRUE(job.Run());

  EXPECT_FLOAT3_EQ(output.translation, 0.f, 0.f, 0.f);
  EXPECT_QUATERNION_EQ(output.rotation, 0.f, 0.f, 0.f, 1.f);
  EXPECT_FLOAT3_EQ(output.scale, 1.f, 1.f, 1.f);

  // One non 0 weights
  layers[0].weight = .8f;

  EXPECT_TRUE(job.Run());

  // Renormalization means that the output will not be the same as the input
  EXPECT_FLOAT3_EQ(output.translation, 2.f, 0.f, 0.f);
  EXPECT_QUATERNION_EQ(output.rotation, .70710677f, 0.f, 0.f, .70710677f);
  EXPECT_FLOAT3_EQ(output.scale, 1.f, 1.f, 1.f);

  // Two non 0 weights
  layers[0].weight = .8f;
  layers[1].weight = .2f;

  EXPECT_TRUE(job.Run());

  EXPECT_FLOAT3_EQ(output.translation, 1.76f, 0.f, .44f);
  EXPECT_QUATERNION_EQ(output.rotation, .6172133f, .1543033f, 0.f, .7715167f);
  EXPECT_FLOAT3_EQ(output.scale, 1.f, 1.f, 1.f);

  // Non normalized weights
  layers[0].weight = 8.f;
  layers[1].weight = 2.f;

  EXPECT_TRUE(job.Run());

  EXPECT_FLOAT3_EQ(output.translation, 1.76f, 0.f, .44f);
  EXPECT_QUATERNION_EQ(output.rotation, .6172133f, .1543033f, 0.f, .7715167f);
  EXPECT_FLOAT3_EQ(output.scale, 1.f, 1.f, 1.f);

  // Non normalized weights
  layers[0].weight = .08f;
  layers[1].weight = .02f;

  EXPECT_TRUE(job.Run());

  EXPECT_FLOAT3_EQ(output.translation, 1.76f, 0.f, .44f);
  EXPECT_QUATERNION_EQ(output.rotation, .6172133f, .1543033f, 0.f, .7715167f);
  EXPECT_FLOAT3_EQ(output.scale, 1.f, 1.f, 1.f);
}