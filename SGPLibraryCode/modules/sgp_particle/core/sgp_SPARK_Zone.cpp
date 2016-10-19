
const float Zone::APPROXIMATION_VALUE = 0.01f;

Zone::Zone(const Vector3D& position) : Registerable(), Transformable()
{
	setPosition(position);
}