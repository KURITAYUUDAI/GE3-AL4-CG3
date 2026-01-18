#include "Camera.h"
#include "WindowsAPI.h"

Camera::Camera()
	: transform_({{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} })
	, fovY_(0.45f)
	, aspectRatio_(static_cast<float>(WindowsAPI::kClientWidth) / static_cast<float>(WindowsAPI::kClientHeight))
	, nearClip_(0.1f)
	, farClip_(100.0f)
	, worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
	, viewMatrix_(Inverse(worldMatrix_))
	, projectionMatrix_(MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_))
	, viewProjectionMatrix_(Multiply(viewMatrix_, projectionMatrix_))
	, billboardMatrix_(MakeIdentity4x4())
{}

void Camera::Initialize()
{


}

void Camera::Update()
{
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
}

void Camera::TransformView()
{
	viewMatrix_ = Inverse(worldMatrix_);
}

void Camera::Transformation()
{
	
	projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);

	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(pi);
	billboardMatrix_ = Multiply(backToFrontMatrix, worldMatrix_);
	billboardMatrix_.m[3][0] = 0.0f;
	billboardMatrix_.m[3][1] = 0.0f;
	billboardMatrix_.m[3][2] = 0.0f;

	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void Camera::Finalize()
{


}

const Matrix4x4 Camera::GetBillboardWorldMatrix(const Vector3& scale, const Vector3& translate) const
{
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 worldMatrix = scaleMatrix * billboardMatrix_ * translateMatrix;

	return worldMatrix;
}