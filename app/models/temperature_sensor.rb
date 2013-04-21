class TemperatureSensor < ActiveRecord::Base
  attr_accessible :arduino_id, :capture_time, :data, :unit

  belongs_to :arduino
  
  FAHRENHEIT = 'fahrenheit'
  CELCIUS    = 'celcius'
  KELVIN     = 'kelvin'

  TEMPERATURE_TYPES = [FAHRENHEIT, CELCIUS, KELVIN]

  validates_presence_of :arduino_id, :data, :unit 

  validate :valid_temperature_type

  def valid_temperature_type
    unless TEMPERATURE_TYPES.include?(self.unit)
      errors.add(:unit, "Unit must be of type : #{TEMPERATURE_TYPES.join(', ')}" )
    end
  end

  def as_json(options = {})
    super(only: [:capture_time, :data, :unit, :created_at])
  end
end
