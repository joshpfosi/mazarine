Rails.application.routes.draw do
  resources :todos, only: [:index, :destroy, :create]
end
