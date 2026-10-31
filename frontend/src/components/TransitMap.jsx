import { MapContainer, TileLayer, Marker, Popup } from 'react-leaflet'

import "leaflet/dist/leaflet.css"
import "../style/components/TransitMap.css"

export default function TransitMap() {
    return (
        <MapContainer
            center={[50.05943, 19.93817]}
            zoom={12.5}
            scrollWheelZoom={false}
            style={{ width: "100%", height: "100%" }} >
            <TileLayer
                attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
                url="https://tile.openstreetmap.org/{z}/{x}/{y}.png"
            />
            <Marker position={[51.505, -0.09]}>
                <Popup>
                    A pretty CSS3 popup. <br /> Easily customizable.
                </Popup>
            </Marker>
        </MapContainer>
    )
}