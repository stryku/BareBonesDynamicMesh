// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/DynamicMeshComponent.h"
#include "CoreMinimal.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "GameFramework/Actor.h"

#include "MyActor.generated.h"

UCLASS()
class BAREBONESDYNAMICMESH_API AMyActor : public AActor {
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AMyActor();

  UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
  UDynamicMeshComponent *MeshComponent;

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  void PostLoad() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

private:
  UE::Geometry::FDynamicMesh3 RegenerateMesh();
  UE::Geometry::FDynamicMesh3 RegenerateMesh2();
  void UpdateMesh(UE::Geometry::FDynamicMesh3 mesh);

  UE::Geometry::FDynamicMesh3 SourceMesh;
};
