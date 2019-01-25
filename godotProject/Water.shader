shader_type spatial;

uniform sampler2D height_map;

void fragment() {
	ALBEDO = texture(height_map, UV).rgb;
}