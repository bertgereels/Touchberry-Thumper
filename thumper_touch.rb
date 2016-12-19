#!/usr/bin/env ruby

require 'listen'
require 'net/http'
require 'json'

class ThumperRestInterface
		
	@@DRIVE_SPEED = 70
	@@ID = 'BD'
	
	def initialize host='http://10.182.34.102:3000'
		@host = host
	end
	
	def strobe
		uri = URI(@host + '/neopixels/effects/strobe/0')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {red: 128, green: 0, blue: 128, delay: 75, id: @@ID }.to_json
		send_request uri, req
	end

	def dim
		uri = URI(@host + '/neopixels/strings/0')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {red: 0, green: 0, blue: 0, id: @@ID }.to_json
		send_request uri, req
	end

	def alarm
                uri = URI(@host + '/alarm')
                req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
                req.body = {action: "toggle", id: @@ID }.to_json
                send_request uri, req
        end
	
	def modified_strobe(rood, groen, blauw, vertraging)
		uri = URI(@host + '/neopixels/effects/strobe/0')
                req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
                req.body = {red: rood, green: groen, blue: blauw, delay: vertraging, id: @@ID }.to_json
                send_request uri, req
	
	end

	def strobe_rood
		modified_strobe(255, 0, 0, 50)
	end
	
	def strobe_blauw
		modified_strobe(0, 0, 255, 50)
	end
		
	def shift
		uri = URI(@host + '/neopixels/effects/shift/0')
                req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
                req.body = {red: 128, green: 0, blue: 128, delay: 25, groupsize: 8, id: @@ID }.to_json
                send_request uri, req
	end 

	def left
		drive @@DRIVE_SPEED, -@@DRIVE_SPEED
	end

	def right
		drive -@@DRIVE_SPEED, @@DRIVE_SPEED
	end

	def forward
		drive @@DRIVE_SPEED, @@DRIVE_SPEED
	end

	def reverse
		drive -@@DRIVE_SPEED, -@@DRIVE_SPEED
		2.times do
			uri = URI(@host + '/alarm')
                	req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
                	req.body = {action: "on", id: @@ID }.to_json
                	send_request uri, req
			sleep(1)
			uri = URI(@host + '/alarm')
                        req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
                        req.body = {action: "off", id: @@ID }.to_json
                        send_request uri, req
			sleep(1)
		end
	end

	def stop
		drive 0, 0
	end

	def drive leftspeed, rightspeed
		uri = URI(@host + '/speed')
		req = Net::HTTP::Post.new(uri, 'Content-Type' => 'application/json')
		req.body = {left_speed: leftspeed, right_speed: rightspeed, id: @@ID }.to_json
		send_request uri, req
	end

	def send_request uri, req
		res = Net::HTTP.start(uri.hostname, uri.port) do |http|
			http.request(req)
		end
	end
	
	private :modified_strobe, :strobe_rood, :strobe_blauw
end

thumper = ThumperRestInterface.new

listener = Listen.to('/tmp/touch') do |modified|
  puts "modified absolute path: #{modified}"
	File.readlines(modified.first).each do |instruction|
		instruction.strip!

		if thumper.respond_to?(instruction.to_sym)
			thumper.send instruction
		else
			thumper.stop
		end

	end

end
listener.start # not blocking

sleep
