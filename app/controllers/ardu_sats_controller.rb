class ArduSatsController < ApplicationController
  # GET /ardu_sats
  # GET /ardu_sats.json
  def index
    @ardu_sats = ArduSat.all

    respond_to do |format|
      format.html # index.html.erb
      format.json { render json: @ardu_sats }
    end
  end

  # GET /ardu_sats/1
  # GET /ardu_sats/1.json
  def show
    @ardu_sat = ArduSat.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.json { render json: @ardu_sat }
    end
  end

  # GET /ardu_sats/new
  # GET /ardu_sats/new.json
  def new
    @ardu_sat = ArduSat.new

    respond_to do |format|
      format.html # new.html.erb
      format.json { render json: @ardu_sat }
    end
  end

  # GET /ardu_sats/1/edit
  def edit
    @ardu_sat = ArduSat.find(params[:id])
  end

  # POST /ardu_sats
  # POST /ardu_sats.json
  def create
    @ardu_sat = ArduSat.new(params[:ardu_sat])

    respond_to do |format|
      if @ardu_sat.save
        format.html { redirect_to @ardu_sat, notice: 'Ardu sat was successfully created.' }
        format.json { render json: @ardu_sat, status: :created, location: @ardu_sat }
      else
        format.html { render action: "new" }
        format.json { render json: @ardu_sat.errors, status: :unprocessable_entity }
      end
    end
  end

  # PUT /ardu_sats/1
  # PUT /ardu_sats/1.json
  def update
    @ardu_sat = ArduSat.find(params[:id])

    respond_to do |format|
      if @ardu_sat.update_attributes(params[:ardu_sat])
        format.html { redirect_to @ardu_sat, notice: 'Ardu sat was successfully updated.' }
        format.json { head :no_content }
      else
        format.html { render action: "edit" }
        format.json { render json: @ardu_sat.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /ardu_sats/1
  # DELETE /ardu_sats/1.json
  def destroy
    @ardu_sat = ArduSat.find(params[:id])
    @ardu_sat.destroy

    respond_to do |format|
      format.html { redirect_to ardu_sats_url }
      format.json { head :no_content }
    end
  end
end
