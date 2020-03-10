extends Control

export(NodePath) var water

func _ready():
	get_node("VBoxContainer/WindSpeed/HSlider").value = get_node(water).tessendorf.wind_speed
	get_node("VBoxContainer/Amplitude/HSlider").value = get_node(water).tessendorf.amplitude
	
func _on_WindSlider_value_changed(value):
	if water:
		get_node(water).tessendorf.wind_speed = value
		get_node(water).tessendorf.calculate()

func _on_AmplitudeSlider_value_changed(value):
	if water:
		get_node(water).tessendorf.amplitude = value
		get_node(water).tessendorf.calculate()
