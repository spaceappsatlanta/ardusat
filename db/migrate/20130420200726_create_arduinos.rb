class CreateArduinos < ActiveRecord::Migration
  def change
    create_table :arduinos do |t|
      t.string :name
      t.string :mac_address
      t.string :email

      t.timestamps
    end
  end
end
