require 'test_helper'

class ArduSatsControllerTest < ActionController::TestCase
  setup do
    @ardu_sat = ardu_sats(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:ardu_sats)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create ardu_sat" do
    assert_difference('ArduSat.count') do
      post :create, ardu_sat: { email: @ardu_sat.email, mac_address: @ardu_sat.mac_address, name: @ardu_sat.name }
    end

    assert_redirected_to ardu_sat_path(assigns(:ardu_sat))
  end

  test "should show ardu_sat" do
    get :show, id: @ardu_sat
    assert_response :success
  end

  test "should get edit" do
    get :edit, id: @ardu_sat
    assert_response :success
  end

  test "should update ardu_sat" do
    put :update, id: @ardu_sat, ardu_sat: { email: @ardu_sat.email, mac_address: @ardu_sat.mac_address, name: @ardu_sat.name }
    assert_redirected_to ardu_sat_path(assigns(:ardu_sat))
  end

  test "should destroy ardu_sat" do
    assert_difference('ArduSat.count', -1) do
      delete :destroy, id: @ardu_sat
    end

    assert_redirected_to ardu_sats_path
  end
end
