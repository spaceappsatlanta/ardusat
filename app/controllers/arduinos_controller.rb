class ArduinosController < ApplicationController
  # GET /arduinos
  # GET /arduinos.json
  def index
    @arduinos = Arduino.all

    respond_to do |format|
      format.html # index.html.erb
      format.json { render json: @arduinos }
    end
  end

  # GET /arduinos/1
  # GET /arduinos/1.json
  def show
    @arduino = Arduino.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.json { render json: @arduino }
    end
  end

  # GET /arduinos/new
  # GET /arduinos/new.json
  def new
    @arduino = Arduino.new

    respond_to do |format|
      format.html # new.html.erb
      format.json { render json: @arduino }
    end
  end

  # GET /arduinos/1/edit
  def edit
    @arduino = Arduino.find(params[:id])
  end

  # POST /arduinos
  # POST /arduinos.json
  def create
    @arduino = Arduino.new(params[:arduino])

    respond_to do |format|
      if @arduino.save
        format.html { redirect_to @arduino, notice: 'Arduino was successfully created.' }
        format.json { render json: @arduino, status: :created, location: @arduino }
      else
        format.html { render action: "new" }
        format.json { render json: @arduino.errors, status: :unprocessable_entity }
      end
    end
  end

  # PUT /arduinos/1
  # PUT /arduinos/1.json
  def update
    @arduino = Arduino.find(params[:id])

    respond_to do |format|
      if @arduino.update_attributes(params[:arduino])
        format.html { redirect_to @arduino, notice: 'Arduino was successfully updated.' }
        format.json { head :no_content }
      else
        format.html { render action: "edit" }
        format.json { render json: @arduino.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /arduinos/1
  # DELETE /arduinos/1.json
  def destroy
    @arduino = Arduino.find(params[:id])
    @arduino.destroy

    respond_to do |format|
      format.html { redirect_to arduinos_url }
      format.json { head :no_content }
    end
  end
end
