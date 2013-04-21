class CreateTemperatureSensors < ActiveRecord::Migration
  def change
    create_table :temperature_sensors do |t|
      t.integer :arduino_id
      t.timestamp :capture_time
      t.float :data
      t.string :unit

      t.timestamps
    end
  end
end
