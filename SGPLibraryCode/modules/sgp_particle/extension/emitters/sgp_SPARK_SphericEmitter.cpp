

SphericEmitter::SphericEmitter(const Vector3D& direction, float angleA, float angleB) :
	Emitter()
{
	setDirection(direction);
	setAngles(angleA, angleB);
}

void SphericEmitter::setDirection(const Vector3D& _direction)
{
	tDirection = _direction;
	computeMatrix();
	this->direction = tDirection; // as tDirection was normalized in computeMatrix()
	notifyForUpdate();
}

void SphericEmitter::setAngles(float angleA, float angleB)
{
	if(angleB < angleA)
		sgp::swapVariables(angleA, angleB);

	angleA = jmin(2.0f * float_Pi, jmax(0.0f, angleA));
	angleB = jmin(2.0f * float_Pi, jmax(0.0f, angleB));

	angleMin = angleA;
	angleMax = angleB;

	cosAngleMin = std::cos(angleMin * 0.5f);
	cosAngleMax = std::cos(angleMax * 0.5f);
}

void SphericEmitter::computeMatrix()
{
	tDirection.Normalize();
	if( (tDirection.x == 0.0f) && (tDirection.y == 0.0f) )
	{
			
		matrix[0] = tDirection.z;
		matrix[1] = 0.0f;
		matrix[2] = 0.0f;
		matrix[3] = 0.0f;
		matrix[4] = tDirection.z;
		matrix[5] = 0.0f;
		matrix[6] = 0.0f;
		matrix[7] = 0.0f;
		matrix[8] = tDirection.z;
	}
	else
	{
		Vector3D axis;
		axis.Cross( tDirection, Vector3D(0.0f,0.0f,1.0f) );

		float cosA = tDirection.z;
		float sinA = -axis.GetLength();
		axis /= -sinA;

		float x = axis.x;
		float y = axis.y;
		float z = axis.z;

		matrix[0] = x * x + cosA * (1.0f - x * x);
		matrix[1] = x * y * (1.0f - cosA) - z * sinA;
		matrix[2] = tDirection.x;
		matrix[3] = x * y * (1.0f - cosA) + z * sinA;
		matrix[4] = y * y + cosA * (1.0f - y * y);
		matrix[5] = tDirection.y;
		matrix[6] = x * z * (1.0f - cosA) - y * sinA;
		matrix[7] = y * z * (1.0f - cosA) + x * sinA;
		matrix[8] = tDirection.z;
	}
}

void SphericEmitter::generateVelocity(Particle& particle, float speed) const
{
	float a = random(cosAngleMax, cosAngleMin);
	float theta = std::acos(a);
	float phi = random(0.0f, 2.0f * float_Pi);

	float sinTheta = std::sin(theta);
	float x = sinTheta * std::cos(phi);
	float y = sinTheta * std::sin(phi);
	float z = std::cos(theta);

	particle.velocity().x = speed * (matrix[0] * x + matrix[1] * y + matrix[2] * z);
	particle.velocity().y = speed * (matrix[3] * x + matrix[4] * y + matrix[5] * z);
	particle.velocity().z = speed * (matrix[6] * x + matrix[7] * y + matrix[8] * z);
}

void SphericEmitter::innerUpdateTransform()
{
	Emitter::innerUpdateTransform();
	tDirection = direction;
	tDirection.RotateWith(getWorldTransform());
	computeMatrix();
}