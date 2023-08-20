#include "MyActor.h"

#include "DynamicMesh/MeshNormals.h"

#include <array>

// Sets default values
AMyActor::AMyActor() {
  // Set this actor to call Tick() every frame.  You can turn this off to
  // improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  MeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(
      TEXT("MeshComponent"), false);
  SetRootComponent(MeshComponent);
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay() {
  Super::BeginPlay();
}

void AMyActor::PostLoad() {
  Super::PostLoad();
  const auto mesh = RegenerateMeshByHandRightHand();
  UpdateMesh(mesh);
}

// Called every frame
void AMyActor::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
}

UE::Geometry::FDynamicMesh3 AMyActor::RegenerateMeshBasedOnObj() {

  UE::Geometry::FDynamicMesh3 mesh;

  mesh.EnableAttributes();

  UE::Geometry::FDynamicMeshNormalOverlay *normalsOverlay =
      mesh.Attributes()->PrimaryNormals();

  UE::Geometry::FDynamicMeshUVOverlay *UVs = mesh.Attributes()->PrimaryUV();

  const std::array vertices{
      FVector3d(1.000000, 1.000000, -1.000000),
      FVector3d(1.000000, -1.000000, -1.000000),
      FVector3d(1.000000, 1.000000, 1.000000),
      FVector3d(1.000000, -1.000000, 1.000000),
      FVector3d(-1.000000, 1.000000, -1.000000),
      FVector3d(-1.000000, -1.000000, -1.000000),
      FVector3d(-1.000000, 1.000000, 1.000000),
      FVector3d(-1.000000, -1.000000, 1.000000),
  };

  const std::array normals{
      FVector3f(-0.0000, 1.0000, -0.0000),
      FVector3f(-0.0000, -0.0000, 1.0000),
      FVector3f(-1.0000, -0.0000, -0.0000),
      FVector3f(-0.0000, -1.0000, -0.0000),
      FVector3f(1.0000, -0.0000, -0.0000),
      FVector3f(-0.0000, -0.0000, -1.0000),
  };

  const std::array textCoords{
      FVector2f(0.625000, 0.500000), FVector2f(0.875000, 0.500000),
      FVector2f(0.875000, 0.750000), FVector2f(0.625000, 0.750000),
      FVector2f(0.375000, 0.750000), FVector2f(0.625000, 1.000000),
      FVector2f(0.375000, 1.000000), FVector2f(0.375000, 0.000000),
      FVector2f(0.625000, 0.000000), FVector2f(0.625000, 0.250000),
      FVector2f(0.375000, 0.250000), FVector2f(0.125000, 0.500000),
      FVector2f(0.375000, 0.500000), FVector2f(0.125000, 0.750000),
  };

  for (auto vertex : vertices) {
    mesh.AppendVertex(vertex * 100);
  }

  for (auto normal : normals) {
    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);
  }

  for (auto textCoord : textCoords) {
    UVs->AppendElement(textCoord);
  }

  struct FaceIndices {
    int vertex;
    int textCoort;
    int normal;
  };

  struct Face {
    FaceIndices a, b, c, d;
  };

  struct Triangle {
    FaceIndices a, b, c;
  };

  const std::array faces{
      Face{FaceIndices{1, 1, 1}, FaceIndices{5, 2, 1}, FaceIndices{7, 3, 1},
           FaceIndices{3, 4, 1}},
      Face{FaceIndices{4, 5, 2}, FaceIndices{3, 4, 2}, FaceIndices{7, 6, 2},
           FaceIndices{8, 7, 2}},
      Face{FaceIndices{8, 8, 3}, FaceIndices{7, 9, 3}, FaceIndices{5, 10, 3},
           FaceIndices{6, 11, 3}},
      Face{FaceIndices{6, 12, 4}, FaceIndices{2, 13, 4}, FaceIndices{4, 5, 4},
           FaceIndices{8, 14, 4}},
      Face{FaceIndices{2, 13, 5}, FaceIndices{1, 1, 5}, FaceIndices{3, 4, 5},
           FaceIndices{4, 5, 5}},
      Face{FaceIndices{6, 11, 6}, FaceIndices{5, 10, 6}, FaceIndices{1, 1, 6},
           FaceIndices{2, 13, 6}},
  };

  for (Face face : faces) {
    Triangle triOne{face.a, face.b, face.c};
    Triangle triTwo{face.a, face.c, face.d};

    int32 tidOne = mesh.AppendTriangle(triOne.a.vertex - 1, triOne.b.vertex - 1,
                                       triOne.c.vertex - 1);
    int32 tidTwo = mesh.AppendTriangle(triTwo.a.vertex - 1, triTwo.b.vertex - 1,
                                       triTwo.c.vertex - 1);

    auto result = normalsOverlay->SetTriangle(
        tidOne, UE::Geometry::FIndex3i(3 * (triOne.a.normal - 1),
                                       3 * (triOne.b.normal - 1) + 1,
                                       3 * (triOne.c.normal - 1) + 2));
    result = normalsOverlay->SetTriangle(
        tidOne, UE::Geometry::FIndex3i(3 * (triTwo.a.normal - 1),
                                       3 * (triTwo.b.normal - 1) + 1,
                                       3 * (triTwo.c.normal - 1) + 2));

    // UVs->SetTriangle(tidOne, UE::Geometry::FIndex3i(triOne.a.textCoort - 1,
    //                                                 triOne.b.textCoort - 1,
    //                                                 triOne.c.textCoort - 1));
    // UVs->SetTriangle(tidTwo, UE::Geometry::FIndex3i(triTwo.a.textCoort - 1,
    //                                                 triTwo.b.textCoort - 1,
    //                                                 triTwo.c.textCoort - 1));
  }

  const auto validityResult =
      mesh.CheckValidity({}, UE::Geometry::EValidityCheckFailMode::Ensure);

  mesh.ReverseOrientation();
  UE::Geometry::FMeshNormals::InitializeOverlayToPerTriangleNormals(
      mesh.Attributes()->PrimaryNormals());

  return mesh;
}

UE::Geometry::FDynamicMesh3 AMyActor::RegenerateMeshByHand() {

  UE::Geometry::FDynamicMesh3 mesh;
  mesh.EnableAttributes();

  UE::Geometry::FDynamicMeshNormalOverlay *normalsOverlay =
      mesh.Attributes()->PrimaryNormals();

  const std::array vertices{FVector3d(-1, -1, -1), FVector3d(1, -1, -1),
                            FVector3d(-1, -1, 1),  FVector3d(1, -1, 1),
                            FVector3d(-1, 1, -1),  FVector3d(1, 1, -1),
                            FVector3d(-1, 1, 1),   FVector3d(1, 1, 1)};

  const std::array normals{FVector3f(0, -1, 0), FVector3f(0, 1, 0),
                           FVector3f(-1, 0, 0), FVector3f(1, 0, 0),
                           FVector3f(0, 0, -1), FVector3f(0, 0, 1)};

  int32 id;
  UE::Geometry::EMeshResult result;

  for (auto vertex : vertices) {
    mesh.AppendVertex(vertex * 100);
  }

  for (auto normal : normals) {
    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);
  }

  // Face 0, 1. Normal 0
  id = mesh.AppendTriangle(0, 1, 2);
  result = normalsOverlay->SetTriangle(id, {0, 1, 2});

  id = mesh.AppendTriangle(3, 1, 2);
  result = normalsOverlay->SetTriangle(id, {0, 1, 2});

  // Face 2, 3. Normal 1
  id = mesh.AppendTriangle(4, 5, 6);
  result = normalsOverlay->SetTriangle(id, {1 * 3, 1 * 3 + 1, 1 * 3 + 2});

  id = mesh.AppendTriangle(7, 5, 6);
  result = normalsOverlay->SetTriangle(id, {1 * 3, 1 * 3 + 1, 1 * 3 + 2});

  // Face 4, 5. Normal 2
  id = mesh.AppendTriangle(4, 6, 0);
  result = normalsOverlay->SetTriangle(id, {2 * 3, 2 * 3 + 1, 2 * 3 + 2});

  id = mesh.AppendTriangle(2, 6, 0);
  result = normalsOverlay->SetTriangle(id, {2 * 3, 2 * 3 + 1, 2 * 3 + 2});

  // Face 6, 7. Normal 3
  id = mesh.AppendTriangle(1, 5, 3);
  result = normalsOverlay->SetTriangle(id, {3 * 3, 3 * 3 + 1, 3 * 3 + 2});

  id = mesh.AppendTriangle(7, 5, 3);
  result = normalsOverlay->SetTriangle(id, {3 * 3, 3 * 3 + 1, 3 * 3 + 2});

  // Face 8, 9. Normal 4
  id = mesh.AppendTriangle(0, 1, 4);
  result = normalsOverlay->SetTriangle(id, {4 * 3, 4 * 3 + 1, 4 * 3 + 2});

  id = mesh.AppendTriangle(5, 1, 4);
  result = normalsOverlay->SetTriangle(id, {4 * 3, 4 * 3 + 1, 4 * 3 + 2});

  // Face 10, 11. Normal 5
  id = mesh.AppendTriangle(2, 3, 6);
  result = normalsOverlay->SetTriangle(id, {5 * 3, 5 * 3 + 1, 5 * 3 + 2});

  id = mesh.AppendTriangle(7, 3, 6);
  result = normalsOverlay->SetTriangle(id, {5 * 3, 5 * 3 + 1, 5 * 3 + 2});

  // Why is this false?
  const auto validityResult =
      mesh.CheckValidity({}, UE::Geometry::EValidityCheckFailMode::ReturnOnly);

  return mesh;
}

UE::Geometry::FDynamicMesh3 AMyActor::RegenerateMeshByHand2() {

  UE::Geometry::FDynamicMesh3 mesh;
  mesh.EnableAttributes();

  UE::Geometry::FDynamicMeshNormalOverlay *normalsOverlay =
      mesh.Attributes()->PrimaryNormals();

  const std::array vertices{FVector3d(-1, -1, -1), FVector3d(1, -1, -1),
                            FVector3d(-1, -1, 1),  FVector3d(1, -1, 1),
                            FVector3d(-1, 1, -1),  FVector3d(1, 1, -1),
                            FVector3d(-1, 1, 1),   FVector3d(1, 1, 1)};

  const std::array normals{FVector3f(0, -1, 0), FVector3f(0, 1, 0),
                           FVector3f(-1, 0, 0), FVector3f(1, 0, 0),
                           FVector3f(0, 0, -1), FVector3f(0, 0, 1)};

  [[maybe_unused]] int32 id;
  [[maybe_unused]] UE::Geometry::EMeshResult result;

  for (auto vertex : vertices) {
    mesh.AppendVertex(vertex * 100);
  }

  for (auto normal : normals) {
    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);

    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);
  }

  // Face 0, 1. Normal 0
  id = mesh.AppendTriangle(0, 2, 3);
  result = normalsOverlay->SetTriangle(id, {0, 1, 2});

  id = mesh.AppendTriangle(0, 3, 1);
  result = normalsOverlay->SetTriangle(id, {3, 4, 5});

  // Face 2, 3. Normal 1
  id = mesh.AppendTriangle(4, 6, 7);
  result = normalsOverlay->SetTriangle(id, {1 * 6, 1 * 6 + 1, 1 * 6 + 2});

  id = mesh.AppendTriangle(4, 7, 5);
  result = normalsOverlay->SetTriangle(id, {1 * 6 + 3, 1 * 6 + 4, 1 * 6 + 5});

  // Face 4, 5. Normal 2
  id = mesh.AppendTriangle(0, 2, 6);
  result = normalsOverlay->SetTriangle(id, {2 * 6, 2 * 6 + 1, 2 * 6 + 2});

  id = mesh.AppendTriangle(0, 6, 4);
  result = normalsOverlay->SetTriangle(id, {2 * 6 + 3, 2 * 6 + 4, 2 * 6 + 5});

  // Face 6, 7. Normal 3
  id = mesh.AppendTriangle(1, 3, 7);
  result = normalsOverlay->SetTriangle(id, {3 * 6, 3 * 6 + 1, 3 * 6 + 2});

  id = mesh.AppendTriangle(1, 7, 5);
  result = normalsOverlay->SetTriangle(id, {3 * 6 + 3, 3 * 6 + 4, 3 * 6 + 5});

  // Face 8, 9. Normal 4
  id = mesh.AppendTriangle(0, 4, 5);
  result = normalsOverlay->SetTriangle(id, {4 * 6, 4 * 6 + 1, 4 * 6 + 2});

  id = mesh.AppendTriangle(0, 5, 1);
  result = normalsOverlay->SetTriangle(id, {4 * 6 + 3, 4 * 6 + 4, 4 * 6 + 5});

  // Face 10, 11. Normal 5
  id = mesh.AppendTriangle(2, 6, 7);
  result = normalsOverlay->SetTriangle(id, {5 * 6, 5 * 6 + 1, 5 * 6 + 2});

  id = mesh.AppendTriangle(2, 7, 3);
  result = normalsOverlay->SetTriangle(id, {5 * 6 + 3, 5 * 6 + 4, 5 * 6 + 5});

  // mesh.ReverseOrientation();
  // UE::Geometry::FMeshNormals::InitializeOverlayToPerTriangleNormals(
  //    mesh.Attributes()->PrimaryNormals());

  // Why is this false?
  const auto validityResult =
      mesh.CheckValidity({}, UE::Geometry::EValidityCheckFailMode::ReturnOnly);

  return mesh;
}

UE::Geometry::FDynamicMesh3 AMyActor::RegenerateMeshByHandRightHand() {

  UE::Geometry::FDynamicMesh3 mesh;
  mesh.EnableAttributes();

  UE::Geometry::FDynamicMeshNormalOverlay *normalsOverlay =
      mesh.Attributes()->PrimaryNormals();

  const std::array vertices{FVector3d(-1, -1, -1), FVector3d(1, -1, -1),
                            FVector3d(-1, -1, 1),  FVector3d(1, -1, 1),
                            FVector3d(-1, 1, -1),  FVector3d(1, 1, -1),
                            FVector3d(-1, 1, 1),   FVector3d(1, 1, 1)};

  const std::array normals{FVector3f(0, -1, 0), FVector3f(0, 1, 0),
                           FVector3f(-1, 0, 0), FVector3f(1, 0, 0),
                           FVector3f(0, 0, -1), FVector3f(0, 0, 1)};

  [[maybe_unused]] int32 id;
  [[maybe_unused]] UE::Geometry::EMeshResult result;

  for (auto vertex : vertices) {
    mesh.AppendVertex(vertex * 100);
  }

  for (auto normal : normals) {
    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);

    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);
    normalsOverlay->AppendElement(normal);
  }

  const auto fixTriangle = [&](int32 v0, int32 v1, int32 v2) {
    const FVector3d vertex0 = vertices[v0];
    const FVector3d vertex1 = vertices[v1];
    const FVector3d vertex2 = vertices[v2];

    const auto normal =
        UE::Geometry::VectorUtil::Normal(vertex0, vertex1, vertex2);
    const auto vertex0100 = vertex0 * 100;
    const auto added = vertex0100 + normal;

    const auto midPoint = (vertex0 + vertex1 + vertex2) / 3.0f;

    const auto dot = normal.Dot(midPoint);
    if (dot < 0) {
      // Need to reverse normal
      return UE::Geometry::FIndex3i(v0, v2, v1);
    } else {
      // Cool normal
      return UE::Geometry::FIndex3i(v0, v1, v2);
    }
  };

  // Face 0, 1. Normal 0
  auto fixed = fixTriangle(0, 2, 3);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {0, 1, 2});

  fixed = fixTriangle(0, 3, 1);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {3, 4, 5});

  //// Face 2, 3. Normal 1
  fixed = fixTriangle(4, 6, 7);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {1 * 6, 1 * 6 + 1, 1 * 6 + 2});

  fixed = fixTriangle(4, 7, 5);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {1 * 6 + 3, 1 * 6 + 4, 1 * 6 + 5});

  // Face 4, 5. Normal 2
  fixed = fixTriangle(0, 2, 6);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {2 * 6, 2 * 6 + 1, 2 * 6 + 2});

  fixed = fixTriangle(0, 6, 4);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {2 * 6 + 3, 2 * 6 + 4, 2 * 6 + 5});

  // Face 6, 7. Normal 3
  fixed = fixTriangle(1, 3, 7);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {3 * 6, 3 * 6 + 1, 3 * 6 + 2});

  fixed = fixTriangle(1, 7, 5);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {3 * 6 + 3, 3 * 6 + 4, 3 * 6 + 5});

  // Face 8, 9. Normal 4
  fixed = fixTriangle(0, 4, 5);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {4 * 6, 4 * 6 + 1, 4 * 6 + 2});

  fixed = fixTriangle(0, 5, 1);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {4 * 6 + 3, 4 * 6 + 4, 4 * 6 + 5});

  // Face 10, 11. Normal 5
  fixed = fixTriangle(2, 6, 7);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {5 * 6, 5 * 6 + 1, 5 * 6 + 2});

  fixed = fixTriangle(2, 7, 3);
  id = mesh.AppendTriangle(fixed[0], fixed[1], fixed[2]);
  result = normalsOverlay->SetTriangle(id, {5 * 6 + 3, 5 * 6 + 4, 5 * 6 + 5});

  // mesh.ReverseOrientation();
  // UE::Geometry::FMeshNormals::InitializeOverlayToPerTriangleNormals(
  //    mesh.Attributes()->PrimaryNormals());

  // Why is this false?
  const auto validityResult =
      mesh.CheckValidity({}, UE::Geometry::EValidityCheckFailMode::ReturnOnly);

  return mesh;
}

void AMyActor::UpdateMesh(UE::Geometry::FDynamicMesh3 mesh) {
  auto root = Cast<UDynamicMeshComponent>(GetRootComponent());

  if (root) {
    *(root->GetMesh()) = mesh;
    root->NotifyMeshUpdated();
  } else {
    UE_LOG(LogTemp, Warning, TEXT("No Mesh Component"));
  }
}
