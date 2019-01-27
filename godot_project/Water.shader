shader_type spatial;

uniform sampler2D height_map;
uniform vec3 off = vec3(-1.0, 0.0, 1.0);
uniform vec2 size = vec2(1.0, 0.0);

vec3 get_normal(in vec2 pos) {
	vec2 texsize = vec2(textureSize(height_map, 0));
	float l = texture(height_map, pos + off.xy / texsize).g; // 01
	float r = texture(height_map, pos + off.zy / texsize).g; // 21
	float u = texture(height_map, pos + off.yx / texsize).g; // 10
	float d = texture(height_map, pos + off.yz / texsize).g; // 12
	
	vec3 va = normalize(vec3(size.xy, r - l));
	vec3 vb = normalize(vec3(size.yx, d - u));

	return normalize(cross(va, vb));
}

void vertex() {
	vec3 data = texture(height_map, UV).rgb;
	NORMAL.xyz = get_normal(UV);
	VERTEX.y = data.y;
	VERTEX.xz += data.xz;
}