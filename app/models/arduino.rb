class Arduino < ActiveRecord::Base
  attr_accessible :email, :mac_address, :name

  has_many :temperature_sensors
end
