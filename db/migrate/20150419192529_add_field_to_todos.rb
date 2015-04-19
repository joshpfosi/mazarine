class AddFieldToTodos < ActiveRecord::Migration
  def change
    add_column :todos, :status, :boolean
    add_column :todos, :bot, :boolean
  end
end
