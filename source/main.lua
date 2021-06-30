local graphics <const> = playdate.graphics
local geometry <const> = playdate.geometry

local game_setup = false

function playdate.update()
  if not game_setup then
    setup()
  end
  
  local dt <const> = 1.0 / playdate.display.getRefreshRate()
  update(dt)
  draw()
end

function playdate.keyPressed(key)
end

function playdate.keyReleased(key)
end

local WORLD_WIDTH <const> = 5.0
local WORLD_HEIGHT <const> = WORLD_WIDTH * 0.6
local WORLD_PIXEL_SCALE <const> = 400.0/WORLD_WIDTH
local FLOOR_WIDTH <const> = 24.0
local FLOOR_HEIGHT <const> = 0.5
local FLOOR_FRICTION <const> = 0.2
local CEILING_WIDTH <const> = 24.0
local CEILING_HEIGHT <const> = 0.1
local CEILING_FRICTION <const> = 0.2
local BOX_COUNT <const> = 4

local world = nil
local ceiling = nil
local floor = nil
local boxes = table.create(BOX_COUNT, 0)
local box_patterns = table.create(BOX_COUNT, 0)
local swing_joint = nil
local swing_box = nil
local selected_box = 1

local MASS_MIN <const> = 50
local MASS_MAX <const> = 120

function setup()
  game_setup = true
  
  -- Setup game refresh rate
  playdate.display.setRefreshRate(30)
  
  -- Setup background color
  playdate.graphics.setBackgroundColor(playdate.graphics.kColorBlack)
  
  -- Seed random number generator
  math.randomseed(playdate.getSecondsSinceEpoch())
  
  -- Create world
  world = playbox.world.new(0.0, 9.81, 10)
  world:setPixelScale(WORLD_PIXEL_SCALE)
  
  -- Create floor
  floor = playbox.body.new(FLOOR_WIDTH, FLOOR_HEIGHT, 0)
  floor:setCenter(WORLD_WIDTH/2, WORLD_HEIGHT)
  floor:setFriction(FLOOR_FRICTION)
  floor:setRotation(0.1)
  world:addBody(floor)
  
  -- Create ceiling
  ceiling = playbox.body.new(CEILING_WIDTH, CEILING_HEIGHT, 0)
  ceiling:setCenter(WORLD_WIDTH/2, CEILING_HEIGHT/2)
  ceiling:setFriction(CEILING_FRICTION)
  world:addBody(ceiling)
  
  -- Create boxes
  for i = 1, BOX_COUNT do
    local box_mass <const> = math.random(MASS_MIN, MASS_MAX)
    local box = playbox.body.new(math.random() * 0.3 + 0.2, math.random() * 0.3 + 0.2, box_mass)
    box:setCenter(math.random() * WORLD_WIDTH, CEILING_HEIGHT)
    box:setFriction(0.8)
    box:setRotation(math.random() * 0.8 + 0.1)
    world:addBody(box)
    boxes[#boxes + 1] = box
    
    -- Box color connected to its mass. Lighter pattern = less mass.
    box_patterns[#box_patterns + 1] = math.max(math.min(1.0 - box_mass/MASS_MAX, 0.9), 0.1)
  end
  
  -- Create swinging box
  swing_box = playbox.body.new(0.1, 1.0, 80)
  swing_box:setCenter(0, CEILING_HEIGHT)
  swing_box:setFriction(1.0)
  world:addBody(swing_box)
  
  -- Create swinging joint
  swing_joint = playbox.joint.new(ceiling, swing_box, WORLD_WIDTH/4, 0)
  swing_joint:setBiasFactor(0.3)
  swing_joint:setSoftness(0.0)
  world:addJoint(swing_joint)
end

function update(dt)
  world:update(dt)
  
  if playdate.buttonJustPressed(playdate.kButtonB) then
    selected_box += 1
    if selected_box > BOX_COUNT then
      selected_box = 1
    end
  end
  
  local box = boxes[selected_box]
  
  if playdate.buttonJustPressed(playdate.kButtonA) then
    box:addForce(0, -9000)
  end
  
  if playdate.buttonIsPressed(playdate.kButtonLeft) then
    box:addForce(-300, 0)
  end
  
  if playdate.buttonIsPressed(playdate.kButtonRight) then
    box:addForce(300, 0)
  end
end

function draw()
  graphics.clear(graphics.kColorWhite)
  graphics.setColor(graphics.kColorBlack)
  
  -- Draw flooring
  local floor_polygon = geometry.polygon.new(floor:getPolygon())
  floor_polygon:close()
  graphics.fillPolygon(floor_polygon)
  
  -- Draw ceiling
  local ceiling_polygon = geometry.polygon.new(ceiling:getPolygon())
  ceiling_polygon:close()
  graphics.fillPolygon(ceiling_polygon)
  
  -- Draw boxes
  graphics.setStrokeLocation(graphics.kStrokeInside)
  for i, box in ipairs(boxes) do
    local box_polygon = geometry.polygon.new(box:getPolygon())
    box_polygon:close()
    graphics.setDitherPattern(box_patterns[i])
    graphics.fillPolygon(box_polygon)
    graphics.setColor(graphics.kColorBlack)
    if i == selected_box then
      graphics.setLineWidth(3)
    else
      graphics.setLineWidth(1)
    end
    graphics.drawPolygon(box_polygon)
  end
  
  
  graphics.setLineWidth(1)
  graphics.setDitherPattern(0.5)
  
  -- Draw swing box
  local swing_box_polygon = geometry.polygon.new(swing_box:getPolygon())
  swing_box_polygon:close()
  graphics.fillPolygon(swing_box_polygon)
  
  -- Draw swing joint
  graphics.setStrokeLocation(graphics.kStrokeCentered)
  local _, _, px1, py1, x2, y2, _, _ = swing_joint:getPoints()
  graphics.setDitherPattern(0.5)
  graphics.drawLine(x2, y2, px1, py1)
  
  -- Draw FPS on device
  if not playdate.isSimulator then
    playdate.drawFPS(380, 15)
  end
end