class CreateArduSats < ActiveRecord::Migration
  def change
    create_table :ardu_sats do |t|
      t.string :name
      t.string :mac_address
      t.string :email

      t.timestamps
    end
  end
end
