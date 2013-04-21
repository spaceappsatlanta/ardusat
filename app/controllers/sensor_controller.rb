class SensorController < ApplicationController

  def temperature
    arduino     = Arduino.find_by_mac_address(params[:arduino])
    sensor      = params[:sensor]

    temperature = new TemperatureSensor(
                                        sensor.slice(:capture_time, :data, :unit)
                                          .merge({arduino_id: arduino.id})
                                        )

    if temperature.save
      render json: temperature
    else
      render json: temperature.errors.messages
    end
  end
end
