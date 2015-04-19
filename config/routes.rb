Rails.application.routes.draw do
  resources :todos, only: [:index, :destroy, :create]

  root to: 'todos#index'
end
