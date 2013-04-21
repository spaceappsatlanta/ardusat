class SensorController < ApplicationController

  def temperature
    arduino  = Arduino.find_by_mac_address(params[:arduino])
    data     = params[:sensor].slice(:capture_time, :data, :unit)

    data.merge!({arduino_id: arduino.id})

    sensor = TemperatureSensor.new(data)

    if sensor.save
      render json: sensor
    else
      render json: sensor.errors.messages
    end
  end

  def temperatures
    arduino = Arduino.find_by_mac_address(params[:arduino])
    sensors = []

    params[:sensors].each do |sensor_data|
      data = sensor_data.slice(:capture_time, :data, :unit)
      data.merge!({arduino_id: arduino.id})

      sensor = TemperatureSensor.new(data)

      if sensor.valid?
        sensors << sensor
      else
        render json: [sensor.errors.messages, params[:sensors]]

        return
      end
    end

    sensors.each { |sensor| sensor.save }

    render json: sensors
  end

end
